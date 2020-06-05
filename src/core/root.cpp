#include "core/root.h"

#include "core/exception.h"
#include "core/real.h"
#include "graphics/render_system.h"
#include "jobs/job_system.h"
#include "log/logger.h"
#include "platform/window.h"

namespace eng
{

Root Root::instance_;

Root::Root()
    : logger_(new Logger{ }),
      job_system_(new JobSystem{ }),
      window_(new Window{ 800.0f, 800.0f }),
      render_system_(new RenderSystem{ 800.0f, 800.0f })
{ }

Root::~Root()
{
    delete render_system_;
    delete window_;
    delete job_system_;

    // delete logger last as other destructors may use it
    delete logger_;
}

Root& Root::instance()
{
    return instance_;
}

JobSystem& Root::job_system()
{
    return *instance_.job_system_;
}

Logger& Root::logger()
{
    return *instance_.logger_;
}

RenderSystem& Root::render_system()
{
    return *instance_.render_system_;
}

Window& Root::window()
{
    return *instance_.window_;
}

}

