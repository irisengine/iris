#include <iostream>
#include <memory>

#include "core/camera.hpp"
#include "core/quaternion.hpp"
#include "graphics/render_system.hpp"
#include "graphics/sprite.hpp"
#include "log/log.hpp"
#include "platform/event.hpp"
#include "platform/window.hpp"

int main()
{
    LOG_DEBUG("sprite_sample", "hello world");

    eng::window w{ 800, 600 };

    auto sprite1 = std::make_shared<eng::sprite>(
        -0.5f,
        0.0f,
        0.4f,
        0.4f,
        eng::vector3{ 0.39f, 0.58f, 0.92f });

    auto sprite2 = std::make_shared<eng::sprite>(
        0.5f,
        0.0f,
        0.4f,
        0.4f,
        eng::vector3{ 0.86f, 0.08f, 0.23f });

    w.add(sprite1);
    w.add(sprite2);

    eng::quaternion rot{ { 0.0f, 0.0f, 1.0f }, 0.0f };
    eng::quaternion delta{ { 0.0f, 0.0f, 1.0f }, 0.02f };

    for(;;)
    {
        if(auto evt = w.pump_event() ; evt)
        {
            if(evt->is_key(eng::key::Q))
            {
                break;
            }
        }

        sprite1->set_orientation(rot);
        sprite2->set_orientation(rot);
        rot *= delta;
        w.render();

    }
    LOG_ERROR("sprite_sample", "goodbye!");

    return 0;
}

