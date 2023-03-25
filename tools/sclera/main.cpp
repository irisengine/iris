////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <cwchar>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include "core/camera.h"
#include "core/colour.h"
#include "core/context.h"
#include "core/looper.h"
#include "core/start.h"
#include "core/vector3.h"
#include "events/event.h"
#include "events/keyboard_event.h"
#include "events/mouse_button_event.h"
#include "graphics/lights/directional_light.h"
#include "graphics/post_processing_description.h"
#include "graphics/render_pipeline.h"
#include "graphics/scene.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"
#include "log/log.h"

#include "metal_gui_renderer.h"

using namespace std::chrono_literals;

namespace
{

/**
 * Helper function to update camera based on user input.
 *
 * @param camera
 *   Camera to update.
 *
 * @param key_map
 *   Map of user pressed keys.
 */
void update_camera(iris::Camera &camera, const std::unordered_map<iris::Key, iris::KeyState> &key_map)
{
    static auto speed = 2.0f;
    iris::Vector3 velocity;

    if (key_map.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        velocity += camera.direction() * speed;
    }

    if (key_map.at(iris::Key::S) == iris::KeyState::DOWN)
    {
        velocity -= camera.direction() * speed;
    }

    if (key_map.at(iris::Key::A) == iris::KeyState::DOWN)
    {
        velocity -= camera.right() * speed;
    }

    if (key_map.at(iris::Key::D) == iris::KeyState::DOWN)
    {
        velocity += camera.right() * speed;
    }

    if (key_map.at(iris::Key::Q) == iris::KeyState::DOWN)
    {
        velocity += camera.right().cross(camera.direction()) * speed;
    }

    if (key_map.at(iris::Key::E) == iris::KeyState::DOWN)
    {
        velocity -= camera.right().cross(camera.direction()) * speed;
    }

    camera.translate(velocity);
}

void go(iris::Context ctx)
{
    LOG_INFO("sclera", "hello sclera");

    static constexpr auto width = 1920u;
    static constexpr auto height = 1080u;

    auto render_pipeline = std::make_unique<iris::RenderPipeline>(
        ctx.material_manager(), ctx.mesh_manager(), ctx.render_target_manager(), width, height);
    auto *scene = render_pipeline->create_scene();
    scene->create_light<iris::DirectionalLight>(iris::Vector3{-1.0f});

    iris::Camera camera{iris::CameraType::PERSPECTIVE, width, height};

    auto *window = ctx.window_manager().create_window(1920, 1080);
    window->set_renderer(std::make_unique<MetalGuiRenderer>(ctx, width, height, scene, camera));

    ctx.texture_manager().blank_texture();
    auto sky_box =
        ctx.texture_manager().create(iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    iris::PostProcessingDescription post_processing_description{.colour_adjust = {iris::ColourAdjustDescription{}}};

    auto *pass = render_pipeline->create_render_pass(scene);
    pass->camera = &camera;
    pass->sky_box = sky_box;
    pass->post_processing_description = post_processing_description;

    window->set_render_pipeline(std::move(render_pipeline));

    std::unordered_map<iris::Key, iris::KeyState> key_map = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    auto right_mouse_down = false;

    iris::Looper looper{
        0ms,
        30ms,
        [&](auto, auto)
        {
            update_camera(camera, key_map);
            return true;
        },
        [&](auto, auto)
        {
            auto running = true;

            auto event = window->pump_event();
            while (event)
            {
                if (event->is_quit() || event->is_key(iris::Key::ESCAPE))
                {
                    running = false;
                }
                else if (event->is_key())
                {
                    const auto keyboard = event->key();
                    key_map[keyboard.key] = keyboard.state;
                }
                else if (event->is_mouse())
                {
                    static const auto sensitivity = 0.0025f;
                    const auto mouse = event->mouse();

                    if (right_mouse_down)
                    {
                        camera.adjust_yaw(mouse.delta_x * sensitivity);
                        camera.adjust_pitch(-mouse.delta_y * sensitivity);
                    }
                }
                else if (event->is_mouse_button((iris::MouseButton::RIGHT)))
                {
                    const auto mouse_button = event->mouse_button();
                    right_mouse_down = mouse_button.state == iris::MouseButtonState::DOWN;
                }

                static_cast<MetalGuiRenderer *>(window->renderer())->handle_input(*event);

                event = window->pump_event();
            }

            window->render();

            return running;
        }};

    looper.run();
}

}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go, true);
    return 0;
}
