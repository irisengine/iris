#include <iostream>
#include <memory>

#include "log/log.hpp"
#include "platform/event_dispatcher.hpp"
#include "platform/window.hpp"
#include "graphics/sprite.hpp"
#include "core/camera.hpp"
#include "graphics/render_system.hpp"
#include "core/quaternion.hpp"

int main()
{
    LOG_DEBUG("window_sample", "hello world");

    bool running = true;

    eng::event_dispatcher dispatcher{
        [&running](const auto &event){ if(event.key == eng::key::Q) running = false; },
        [](const auto&) { },
    };

    auto rs = std::make_shared<eng::render_system>(
        std::make_shared<eng::camera>(),
        std::make_shared<eng::window>(dispatcher, 800, 600));

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

    rs->add(sprite1);
    rs->add(sprite2);

    eng::quaternion rot{ { 0.0f, 0.0f, 1.0f }, 0.0f };
    eng::quaternion delta{ { 0.0f, 0.0f, 1.0f }, 0.02f };

    while(running)
    {
        sprite1->set_orientation(rot);
        sprite2->set_orientation(rot);
        rot *= delta;
        rs->render();

    }
    LOG_ERROR("window_sample", "goodbye!");

    return 0;
}

