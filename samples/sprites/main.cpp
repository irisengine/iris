#include <iostream>
#include <memory>

#include "core/camera.h"
#include "core/quaternion.h"
#include "graphics/render_system.h"
#include "graphics/sprite.h"
#include "graphics/texture.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/window.h"

void go()
{
    LOG_DEBUG("sprite_sample", "hello world");

    eng::Window w{ 800, 600 };

    auto *sprite1 = w.create(
        0,
        0,
        100,
        100,
        eng::Vector3{ 0.39f, 0.58f, 0.92f });

    auto *sprite2 = w.create(
        0.0f,
        150.0f,
        100,
        100,
        eng::Vector3{ 0.86f, 0.08f, 0.23f });

    auto *sprite3 = w.create(
        0.0f,
        -150.0f,
        100.0f,
        100.0f,
        eng::Vector3{ 1.0f, 1.0f, 1.0f },
        eng::Texture("./circle.png"));

    eng::Quaternion rot{ { 0.0f, 0.0f, 1.0f }, 0.0f };
    eng::Quaternion delta{ { 0.0f, 0.0f, 1.0f }, 0.02f };

    for(;;)
    {
        if(auto evt = w.pump_event() ; evt)
        {
            if(evt->is_key(eng::Key::Q))
            {
                break;
            }
        }

        sprite1->set_orientation(rot);
        sprite2->set_orientation(rot);
        sprite3->set_orientation(rot);
        rot *= delta;
        w.render();

    }
    LOG_ERROR("sprite_sample", "goodbye!");
}

int main()
{
    try
    {
        go();
    }
    catch(eng::Exception &e)
    {
        LOG_ERROR("sprite_sample", e.what());
        LOG_ERROR("sprite_sample", e.stack_trace());
    }
    catch(...)
    {
        LOG_ERROR("sprite_sample", "unknown exception");
    }
}

