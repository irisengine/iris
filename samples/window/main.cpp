#include <iostream>

#include "log/log.hpp"
#include "platform/window.hpp"

int main()
{
    LOG_DEBUG("window_sample", "hello world");

    eng::window w{ 800, 600 };

    for(;;)
    {
        if(auto evt = w.pump_event() ; evt)
        {
            if(evt->is_key(eng::key::Q))
            {
                break;
            }
        }

        w.render();
    }
    LOG_ERROR("window_sample", "goodbye!");

    return 0;
}

