////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>
#include <optional>

#include "core/camera.h"
#include "core/colour.h"
#include "core/context.h"
#include "core/looper.h"
#include "core/start.h"
#include "events/event.h"
#include "events/keyboard_event.h"
#include "graphics/post_processing_description.h"
#include "graphics/render_pipeline.h"
#include "graphics/scene.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"
#include "log/log.h"

using namespace std::chrono_literals;

void go(iris::Context ctx)
{
    LOG_INFO("sclera", "hello sclera");

    auto *window = ctx.window_manager().create_window(1920, 1080);
    iris::Camera camera{iris::CameraType::ORTHOGRAPHIC, window->width(), window->height()};

    auto render_pipeline = std::make_unique<iris::RenderPipeline>(
        ctx.material_manager(), ctx.mesh_manager(), ctx.render_target_manager(), window->width(), window->height());
    auto *scene = render_pipeline->create_scene();

    ctx.texture_manager().blank_texture();
    auto sky_box =
        ctx.texture_manager().create(iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    auto *pass = render_pipeline->create_render_pass(scene);
    pass->camera = &camera;
    pass->sky_box = sky_box;

    window->set_render_pipeline(std::move(render_pipeline));

    iris::Looper looper{
        0ms,
        30ms,
        [](auto, auto) { return true; },
        [window](auto, auto)
        {
            auto running = true;

            auto event = window->pump_event();
            while (event)
            {
                if (event->is_quit() || event->is_key(iris::Key::ESCAPE))
                {
                    running = false;
                }

                event = window->pump_event();
            }

            window->render();

            return running;
        }};

    looper.run();
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
    return 0;
}
