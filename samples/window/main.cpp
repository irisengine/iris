#include <iostream>

#include "log/log.hpp"
#include "platform/event_dispatcher.hpp"
#include "platform/window.hpp"

int main()
{
    LOG_DEBUG("window_sample", "hello world");

    bool running = true;

    eng::event_dispatcher dispatcher{
        [&running](const auto &event){ if(event.key == eng::key::Q) running = false; },
        [](const auto&) { },
    };

    eng::window w{ dispatcher, 800, 600 };

    while(running)
    {
        w.render();
    }
    LOG_ERROR("window_sample", "goodbye!");

    return 0;
}

