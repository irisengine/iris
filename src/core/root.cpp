#include "core/root.h"

#include <memory>

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
    : logger_(std::make_unique<Logger>()),
      job_system_(std::make_unique<JobSystem>()),
      window_(std::make_unique<Window>(800.0f, 800.0f)),
      render_system_(std::make_unique<RenderSystem>(window_->width(), window_->height()))
{ }

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

