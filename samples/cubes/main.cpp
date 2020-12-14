#include <map>

#include "core/camera.h"
#include "core/root.h"
#include "graphics/mesh_factory.h"
#include "graphics/pipeline.h"
#include "graphics/render_entity.h"
#include "graphics/scene.h"
#include "graphics/stage.h"
#include "log/log.h"
#include "platform/keyboard_event.h"
#include "platform/start.h"
#include "platform/window.h"

void go(int, char **)
{
    LOG_DEBUG("cube_sample", "hello world");

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

    auto scene = std::make_unique<iris::Scene>();

    scene->create(
        iris::RenderGraph(),
        iris::mesh_factory::cube({1.0f, 0.0f, 0.0f}),
        iris::Vector3{-20.0f, 0.0f, 0.0f},
        iris::Vector3{10.0f});

    scene->create(
        iris::RenderGraph(),
        iris::mesh_factory::cube({0.0f, 0.0f, 1.0f}),
        iris::Vector3{20.0f, 0.0f, 0.0f},
        iris::Vector3{10.0f});

    auto stage = std::make_unique<iris::Stage>(std::move(scene), camera);
    iris::Pipeline pipeline(std::move(stage));

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

        rs.render(pipeline);
    }
    LOG_ERROR("cube_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    try
    {
        iris::start_debug(argc, argv, go);
    }
    catch (iris::Exception &e)
    {
        LOG_ERROR("cube_sample", e.what());
        LOG_ERROR("cube_sample", e.stack_trace());
    }
    catch (...)
    {
        LOG_ERROR("cube_sample", "unknown exception");
    }

    return 0;
}
