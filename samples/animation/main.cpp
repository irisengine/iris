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

    auto stage = std::make_unique<iris::Stage>(std::move(scene), camera);
    iris::Pipeline pipeline{std::move(stage)};

    iris::Transform light{{500.0f, 100.0f, 0.0f}, {}, {1.0f}};

    zombie->skeleton().set_animation("Zombie|ZombieWalk");

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

        rs.render(pipeline);

        zombie->skeleton().advance();
    }

    LOG_ERROR("animation_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
