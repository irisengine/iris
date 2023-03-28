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
void update_camera(iris::Camera &camera, bool track_up, bool track_down, bool track_left, bool track_right)
{
    static auto speed = 2.0f;
    iris::Vector3 velocity;

    // if (key_map.at(iris::Key::W) == iris::KeyState::DOWN)
    //{
    //     velocity += camera.direction() * speed;
    // }

    // if (key_map.at(iris::Key::S) == iris::KeyState::DOWN)
    //{
    //     velocity -= camera.direction() * speed;
    // }

    if (track_left)
    {
        velocity -= camera.right() * speed;
    }

    if (track_right)
    {
        velocity += camera.right() * speed;
    }

    if (track_up)
    {
        velocity += camera.right().cross(camera.direction()) * speed;
    }

    if (track_down)
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
    // camera.translate({0.0f, 10.0f, 0.0f});
    // camera.look_at({});

    auto *window = ctx.window_manager().create_window(1920, 1080);
    window->set_renderer(std::make_unique<MetalGuiRenderer>(ctx, window, scene, camera));

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

    auto left_mouse_down = false;
    auto track_up = false;
    auto track_down = false;
    auto track_left = false;
    auto track_right = false;

    iris::Looper looper{
        0ms,
        30ms,
        [&](auto, auto)
        {
            update_camera(camera, track_up, track_down, track_left, track_right);
            return true;
        },
        [&](auto, auto)
        {
            auto running = true;

            auto event = window->pump_event();
            while (event)
            {
                auto *metal_renderer = static_cast<MetalGuiRenderer *>(window->renderer());
                metal_renderer->handle_input(*event);

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

                    if (left_mouse_down && (key_map[iris::Key::OPTION] == iris::KeyState::DOWN) &&
                        (key_map[iris::Key::COMMAND] == iris::KeyState::DOWN))
                    {
                        static constexpr auto threshold = 0.5f;

                        track_up = mouse.delta_y > threshold;
                        track_down = mouse.delta_y < -threshold;
                        track_left = mouse.delta_x > threshold;
                        track_right = mouse.delta_x < -threshold;
                    }
                    else if (left_mouse_down && (key_map[iris::Key::OPTION] == iris::KeyState::DOWN))
                    {
                        camera.adjust_yaw(mouse.delta_x * sensitivity);
                        camera.adjust_pitch(-mouse.delta_y * sensitivity);
                    }
                    else
                    {
                        track_up = false;
                        track_down = false;
                        track_left = false;
                        track_right = false;
                    }
                }
                else if (event->is_mouse_button((iris::MouseButton::LEFT)))
                {
                    const auto mouse_button = event->mouse_button();
                    left_mouse_down = mouse_button.state == iris::MouseButtonState::DOWN;
                }
                else if (event->is_scroll_wheel() && !metal_renderer->is_mouse_captured())
                {
                    const auto scroll = event->scroll_wheel();
                    camera.translate(camera.direction() * scroll.delta_y);
                }

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
