#include <iostream>
#include <memory>

#include "graphics/render_system.h"
#include "graphics/sprite.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/window.h"

int go()
{
    LOG_DEBUG("font_sample", "hello world");

    eng::Window w{ 800, 600 };

    w.create("Helvetica", 12, { 1, 1, 1 }, "hello world", 0, 0);

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

    LOG_ERROR("font_sample", "goodbye!");

    return 0;
}

int main()
{
    try
    {
        go();
    }
    catch(eng::Exception &e)
    {
        LOG_ERROR("font_sample", e.what());
        LOG_ERROR("font_sample", e.stack_trace());
    }
    catch(...)
    {
        LOG_ERROR("font_sample", "unknown exception");
    }
}

