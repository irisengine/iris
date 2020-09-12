#include <iostream>
#include <memory>

#include "core/root.h"
#include "core/vector3.h"
#include "graphics/font.h"
#include "graphics/render_system.h"
#include "graphics/sprite.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/start.h"

int go(int, char**)
{
    LOG_DEBUG("font_sample", "hello world");

    auto &rs = iris::Root::instance().render_system();

    rs.create<iris::Font>("Helvetica", 12, "hello world", iris::Vector3{ 1.0f, 1.0f, 1.0f });

    for(;;)
    {
        if(auto evt = iris::Root::instance().window().pump_event() ; evt)
        {
            if(evt->is_key(iris::Key::Q))
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
        iris::start_debug(argc, argv, go);
    }
    catch(iris::Exception &e)
    {
        LOG_ERROR("font_sample", e.what());
        LOG_ERROR("font_sample", e.stack_trace());
    }
    catch(...)
    {
        LOG_ERROR("font_sample", "unknown exception");
    }
}

