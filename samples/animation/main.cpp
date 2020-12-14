#include <cmath>
#include <map>
#include <memory>

#include "core/camera.h"
#include "core/root.h"
#include "core/transform.h"
#include "graphics/material.h"
#include "graphics/mesh_factory.h"
#include "graphics/pipeline.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/scene.h"
#include "graphics/stage.h"
#include "log/log.h"
#include "physics/box_collision_shape.h"
#include "physics/physics_system.h"
#include "platform/keyboard_event.h"
#include "platform/resource_loader.h"
#include "platform/start.h"
#include "platform/window.h"

void go(int, char **)
{
    LOG_DEBUG("animation_sample", "hello world");

    iris::ResourceLoader::instance().set_root_directory("assets");

    std::map<iris::Key, iris::KeyState> key_map{
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    auto &rs = iris::Root::instance().render_system();
    auto &ps = iris::Root::instance().physics_system();

    iris::Camera camera{iris::CameraType::PERSPECTIVE, 800.0f, 800.0f};
    iris::Camera screen_camera{iris::CameraType::ORTHOGRAPHIC, 800.0f, 800.0f};

    auto scene = std::make_unique<iris::Scene>();
    iris::RenderGraph render_graph{};

    auto *texture = render_graph.create<iris::TextureNode>("ZombieTexture.png");

    render_graph.render_node()->set_colour_input(texture);

    auto [mesh, skeleton] = iris::mesh_factory::load("Zombie.fbx");

    auto *zombie = scene->create(
        std::move(render_graph),
        std::move(mesh),
        iris::Vector3{0.0f, 0.0f, 0.0f},
        iris::Quaternion{},
        iris::Vector3{0.035f},
        skeleton);

    auto *debug_draw = scene->create(
        iris::RenderGraph{},
        iris::mesh_factory::empty(),
        iris::Vector3{},
        iris::Vector3{1.0f});

    ps.enable_debug_draw(debug_draw);

    auto stage = std::make_unique<iris::Stage>(std::move(scene), camera);
    iris::Pipeline pipeline{std::move(stage)};

    iris::Transform light{{500.0f, 100.0f, 0.0f}, {}, {1.0f}};

    std::vector<std::string> animations{
        "Zombie|ZombieWalk",
        "Zombie|ZombieBite",
        "Zombie|ZombieCrawl",
        "Zombie|ZombieIdle",
        "Zombie|ZombieRun"};

    auto animation = 0u;

    zombie->skeleton().set_animation(animations[animation]);

    // offsets and scales for bones we want to add rigid bodies to, these were
    // all handcrafted
    std::map<std::string, std::tuple<iris::Vector3, iris::Vector3>>
        hit_box_data = {
            {"Head", {{}, {1.0f}}},
            {"HeadTop_End", {{0.0f, -0.2f, 0.0f}, {1.0f}}},

            {"RightArm", {{}, {1.0f}}},
            {"RightForeArm", {{}, {1.0f, 2.5f, 1.0f}}},
            {"RightHand", {{}, {1.0f}}},

            {"LeftArm", {{}, {1.0f}}},
            {"LeftForeArm", {{}, {1.0f, 2.5f, 1.0f}}},
            {"LeftHand", {{}, {1.0f}}},

            {"Spine", {{}, {2.0f, 1.0f, 1.0f}}},
            {"Spine1", {{}, {2.0f, 1.0f, 1.0f}}},
            {"Spine2", {{}, {2.0f, 1.0f, 1.0f}}},
            {"Hips", {{}, {1.0f}}},

            {"LeftUpLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 2.5f, 1.0f}}},
            {"LeftLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 3.0f, 1.0f}}},
            {"LeftFoot", {{}, {1.0f, 2.5f, 1.0f}}},

            {"RightUpLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 2.5f, 1.0f}}},
            {"RightLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 3.0f, 1.0f}}},
            {"RightFoot", {{}, {1.0f, 2.5f, 1.0f}}}};

    std::map<std::string, std::tuple<std::size_t, iris::RigidBody *>> hit_boxes;

    // iterate all bones and create hit boxes for those we have data for
    for (auto i = 0u; i < zombie->skeleton().bones().size(); ++i)
    {
        const auto &bone = zombie->skeleton().bone(i);

        const auto box_data = hit_box_data.find(bone.name());
        if (box_data != std::end(hit_box_data))
        {
            const auto &[pos_offset, scale_offset] = box_data->second;

            // get bone transform in world space
            const auto transform = iris::Transform{
                zombie->transform() * zombie->skeleton().transforms()[i] *
                iris::Matrix4::invert(bone.offset())};

            // create hit box
            hit_boxes[box_data->first] = {
                i,
                ps.create_rigid_body(
                    iris::Vector3{},
                    std::make_unique<iris::BoxCollisionShape>(scale_offset),
                    iris::RigidBodyType::GHOST)};

            // calculate hit box location after offset is applied
            const auto offset =
                transform * iris::Matrix4::make_translate(pos_offset);

            // update hit box
            std::get<1>(hit_boxes[box_data->first])
                ->reposition(offset.translation(), transform.rotation());
            std::get<1>(hit_boxes[box_data->first])->set_name(box_data->first);
        }
    }

    for (;;)
    {
        if (auto evt = iris::Root::instance().window().pump_event(); evt)
        {
            if (evt->is_key(iris::Key::ESCAPE))
            {
                break;
            }
            else if (evt->is_key())
            {
                const auto keyboard = evt->key();
                key_map[keyboard.key] = keyboard.state;

                if ((keyboard.key == iris::Key::SPACE) &&
                    (keyboard.state == iris::KeyState::UP))
                {
                    animation = (animation + 1u) % animations.size();
                    zombie->skeleton().set_animation(animations[animation]);
                }
            }
            else if (evt->is_mouse())
            {
                static const auto sensitivity = 0.0025f;
                const auto mouse = evt->mouse();

                camera.adjust_yaw(mouse.delta_x * sensitivity);
                camera.adjust_pitch(-mouse.delta_y * sensitivity);
            }
        }

        static auto speed = 2.0f;
        iris::Vector3 velocity;

        if (key_map[iris::Key::W] == iris::KeyState::DOWN)
        {
            velocity += camera.direction() * speed;
        }

        if (key_map[iris::Key::S] == iris::KeyState::DOWN)
        {
            velocity -= camera.direction() * speed;
        }

        if (key_map[iris::Key::A] == iris::KeyState::DOWN)
        {
            velocity -= camera.right() * speed;
        }

        if (key_map[iris::Key::D] == iris::KeyState::DOWN)
        {
            velocity += camera.right() * speed;
        }

        if (key_map[iris::Key::Q] == iris::KeyState::DOWN)
        {
            velocity += camera.right().cross(camera.direction()) * speed;
        }

        if (key_map[iris::Key::E] == iris::KeyState::DOWN)
        {
            velocity -= camera.right().cross(camera.direction()) * speed;
        }

        camera.translate(velocity);

        light.set_matrix(
            iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) *
            light.matrix());

        rs.set_light_position(light.translation());

        zombie->skeleton().advance();

        // update hit boxes
        for (auto &[name, data] : hit_boxes)
        {
            auto &[index, body] = data;

            // get hotbox transform in world space
            const auto transform = iris::Transform{
                zombie->transform() * zombie->skeleton().transforms()[index] *
                iris::Matrix4::invert(zombie->skeleton().bone(index).offset())};
            const auto offset =
                transform *
                iris::Matrix4::make_translate(std::get<0>(hit_box_data[name]));

            body->reposition(offset.translation(), transform.rotation());
        }

        ps.step(std::chrono::milliseconds(33));
        rs.render(pipeline);
    }

    LOG_ERROR("animation_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
