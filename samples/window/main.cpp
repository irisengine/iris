#include <iostream>

#include "log/log.h"
#include "platform/window.h"

int main()
{
    LOG_DEBUG("window_sample", "hello world");

    eng::Window w{ 800, 600 };

    for(;;)
    {
        if(auto evt = w.pump_event() ; evt)
        {
            if(evt->is_key(eng::Key::Q))
            {
                break;
            }
        }

        w.render();
    }
    LOG_ERROR("window_sample", "goodbye!");

    return 0;
}

