#include <iostream>

#include "log/log.h"
#include "platform/start.h"
#include "platform/window.h"

void go(int, char **)
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
}

int main(int argc, char **argv)
{
    try
    {
        eng::start(argc, argv, go);
    }
    catch(eng::Exception &e)
    {
        LOG_ERROR("window_sample", e.what());
        LOG_ERROR("window_sample", e.stack_trace());
    }
    catch(...)
    {
        LOG_ERROR("window_sample", "unknown exception");
    }

    return 0;
}

