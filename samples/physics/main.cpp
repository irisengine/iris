#include <chrono>
#include <cmath>
#include <map>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "core/colour.h"
#include "core/resource_loader.h"
#include "core/start.h"
#include "core/transform.h"
#include "core/window.h"
#include "events/keyboard_event.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/mesh_factory.h"
#include "graphics/pipeline.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/scene.h"
#include "graphics/stage.h"
#include "log/log.h"
#include "physics/basic_character_controller.h"
#include "physics/box_collision_shape.h"
#include "physics/physics_system.h"
#include "physics/rigid_body.h"

void handle_left_touch(
    iris::Vector3 &walk_direction,
    const iris::Vector3 &touch_origin,
    const iris::Vector3 &touch_pos,
    const iris::Camera &camera)
{
    const auto touch_direction = touch_pos - touch_origin;

    if (touch_direction.magnitude() > 40.0f)
    {
        const auto forward_angle = std::acos(
            iris::Vector3::normalise(touch_direction).dot({0.0f, 1.0f, 0.0f}));
        if (std::abs(forward_angle) < 0.3f)
        {
            walk_direction -= camera.direction();
        }
        else if (std::abs(forward_angle) > 2.8f)
        {
            walk_direction += camera.direction();
        }

        const auto strafe_angle = std::acos(
            iris::Vector3::normalise(touch_direction).dot({1.0f, 0.0f, 0.0f}));
        if (std::abs(strafe_angle) < 0.3f)
        {
            walk_direction += camera.right();
        }
        else if (std::abs(strafe_angle) > 2.8f)
        {
            walk_direction -= camera.right();
        }
    }
    else
    {
        walk_direction = {};
    }
}

void go(int, char **)
{
    LOG_DEBUG("physics_sample", "hello world");

    iris::ResourceLoader::instance().set_root_directory("assets");

    std::map<iris::Key, iris::KeyState> key_map{
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
        {iris::Key::SPACE, iris::KeyState::UP},
    };

    iris::Window window{800u, 800u};
    iris::PhysicsSystem ps{};
    iris::Camera camera{iris::CameraType::PERSPECTIVE, 800.0f, 800.0f};

    std::vector<std::tuple<iris::RenderEntity *, iris::RigidBody *>> boxes;

    iris::RenderGraph floor_graph{};
    floor_graph.render_node()->set_specular_amount_input(
        floor_graph.create<iris::ValueNode<float>>(0.0f));

    auto scene = std::make_unique<iris::Scene>();
    scene->create_entity(
        &floor_graph,
        iris::mesh_factory::cube({1.0f, 1.0f, 1.0f}),
        iris::Transform{
            iris::Vector3{0.0f, -50.0f, 0.0f},
            {},
            iris::Vector3{500.0f, 50.0f, 500.0f}});

    scene->set_ambient_light({0.01f, 0.01f, 0.01f});
    scene->create_light<iris::DirectionalLight>(
        iris::Vector3{0.0f, -1.0f, -1.0f}, true);
    auto *light = scene->create_light<iris::PointLight>(
        iris::Vector3{0.0f, 1.0f, -10.0f});

    ps.create_rigid_body(
        iris::Vector3{0.0f, -50.0f, 0.0f},
        std::make_unique<iris::BoxCollisionShape>(
            iris::Vector3{500.0f, 50.0f, 500.0f}),
        iris::RigidBodyType::STATIC);

    iris::RenderGraph graph{};
    graph.render_node()->set_colour_input(
        graph.create<iris::TextureNode>("crate.png"));
    graph.render_node()->set_specular_amount_input(
        graph.create<iris::ComponentNode>(
            graph.create<iris::TextureNode>("crate_specular.png"), "r"));

    auto width = 21u;
    auto height = 10u;

    for (auto y = 0u; y < height; ++y)
    {
        for (auto x = 0u; x < width; ++x)
        {
            const iris::Vector3 pos{
                static_cast<float>(x), static_cast<float>(y), 0.0f};
            static const iris::Vector3 half_size{0.5f, 0.5f, 0.5f};
            auto colour = ((y * height) + x + (y % 2)) % 2 == 0
                              ? iris::Colour{1.0f, 0.0f, 0.0f}
                              : iris::Colour{0.0f, 0.0f, 1.0f};

            boxes.emplace_back(
                scene->create_entity(
                    &graph,
                    iris::mesh_factory::cube({1.0f, 1.0f, 1.0f}),
                    iris::Transform{pos, {}, half_size}),
                ps.create_rigid_body(
                    pos,
                    std::make_unique<iris::BoxCollisionShape>(half_size),
                    iris::RigidBodyType::NORMAL));
        }
    }

    iris::Pipeline pipeline{};
    pipeline.add_stage(std::move(scene), camera);

    auto *character_controller =
        ps.create_character_controller<iris::BasicCharacterController>(&ps);

    auto frame_start = std::chrono::high_resolution_clock::now();

    iris::Vector3 left_touch_origin;
    std::uintptr_t left_touch_id = 0u;
    iris::Vector3 right_touch_origin;
    std::uintptr_t right_touch_id = 0u;
    iris::Vector3 walk_direction{};
    auto delta_x = 0.0f;
    auto delta_y = 0.0f;

    iris::Transform light_transform{light->position(), {}, {1.0f}};

    for (;;)
    {
        if (auto evt = window.pump_event(); evt)
        {
            if (evt->is_key(iris::Key::ESCAPE))
            {
                break;
            }
            else if (evt->is_key())
            {
                const auto keyboard = evt->key();
                key_map[keyboard.key] = keyboard.state;
            }
            else if (evt->is_mouse())
            {
                static const auto sensitivity = 0.0025f;
                const auto mouse = evt->mouse();

                camera.adjust_yaw(mouse.delta_x * sensitivity);
                camera.adjust_pitch(-mouse.delta_y * sensitivity);
            }
            else if (evt->is_touch())
            {
                const auto touch = evt->touch();
                switch (touch.type)
                {
                    case iris::TouchType::BEGIN:
                        if (touch.x < window.width() / 2.0f)
                        {
                            left_touch_origin = {touch.x, touch.y, 0.0f};
                            left_touch_id = touch.id;
                        }
                        else
                        {
                            right_touch_origin = {touch.x, touch.y, 0.0f};
                            right_touch_id = touch.id;
                        }
                        break;
                    case iris::TouchType::MOVE:
                    {
                        const iris::Vector3 touch_pos{touch.x, touch.y, 0.0f};

                        if (touch.id == left_touch_id)
                        {
                            handle_left_touch(
                                walk_direction,
                                left_touch_origin,
                                touch_pos,
                                camera);
                        }
                        else
                        {
                            static const auto sensitivity = 0.0025f;

                            const auto delta = touch_pos - right_touch_origin;
                            delta_x = delta.x;
                            delta_y = delta.y;
                            right_touch_origin = touch_pos;

                            camera.adjust_yaw(delta_x * sensitivity);
                            camera.adjust_pitch(-delta_y * sensitivity);
                        }

                        break;
                    }
                    case iris::TouchType::END: walk_direction = {}; break;
                }
            }
        }

        if (key_map[iris::Key::W] == iris::KeyState::DOWN)
        {
            walk_direction += camera.direction();
        }

        if (key_map[iris::Key::S] == iris::KeyState::DOWN)
        {
            walk_direction -= camera.direction();
        }

        if (key_map[iris::Key::A] == iris::KeyState::DOWN)
        {
            walk_direction -= camera.right();
        }

        if (key_map[iris::Key::D] == iris::KeyState::DOWN)
        {
            walk_direction += camera.right();
        }

        if (key_map[iris::Key::SPACE] == iris::KeyState::DOWN)
        {
            character_controller->jump();
        }

        walk_direction.normalise();

        character_controller->set_walk_direction(walk_direction);

        auto frame_end = std::chrono::high_resolution_clock::now();

        ps.step(std::chrono::duration_cast<std::chrono::milliseconds>(
            frame_end - frame_start));

        frame_start = std::chrono::high_resolution_clock::now();

        const auto cam_pos = character_controller->position();

        camera.translate(
            cam_pos - camera.position() + iris::Vector3{0.0f, 0.5f, 0.0f});

        light_transform.set_matrix(
            iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) *
            light_transform.matrix());
        light->set_position(light_transform.translation());

        for (const auto &[g, p] : boxes)
        {
            g->set_position(p->position());
            g->set_orientation(p->orientation());
        }

        window.render(pipeline);

#if !defined(IRIS_PLATFORM_IOS)
        walk_direction = {};
#endif
    }
    LOG_ERROR("physics_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
