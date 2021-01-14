#include <iostream>
#include <memory>

#include "core/camera.h"
#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/pipeline.h"
#include "graphics/render_system.h"
#include "graphics/stage.h"
#include "graphics/text_factory.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/start.h"
#include "platform/window.h"

int go(int, char **)
{
    LOG_DEBUG("text_rendering_sample", "hello world");

    iris::Window window{800.0f, 800.0f};

    iris::Camera screen_camera{iris::CameraType::ORTHOGRAPHIC, 800.0f, 800.0f};

    iris::Pipeline pipeline{};
    pipeline.add_stage(
        iris::text_factory::create(
            "Helvetica", 12, "hello world", iris::Colour{1.0f, 1.0f, 1.0f}),
        screen_camera);

    for (;;)
    {
        if (auto evt = window.pump_event(); evt)
        {
            if (evt->is_key(iris::Key::Q))
            {
                break;
            }
        }

        window.render(pipeline);
    }

    LOG_ERROR("text_rendering_sample", "goodbye!");

    return 0;
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
