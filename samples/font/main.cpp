#include <iostream>
#include <memory>

#include "core/root.h"
#include "graphics/render_system.h"
#include "graphics/sprite.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/start.h"

int go(int, char**)
{
    LOG_DEBUG("font_sample", "hello world");

    auto &rs = eng::Root::instance().render_system();

    rs.create("Helvetica", 12, { 1, 1, 1 }, "hello world", 0, 0);

    for(;;)
    {
        if(auto evt = eng::Root::instance().window().pump_event() ; evt)
        {
            if(evt->is_key(eng::Key::Q))
            {
                break;
            }
        }

        rs.render();
    }

    LOG_ERROR("font_sample", "goodbye!");

    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        eng::start_debug(argc, argv, go);
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

