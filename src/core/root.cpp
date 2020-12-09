#include "core/root.h"

#include <memory>

#include "core/exception.h"
#include "graphics/render_system.h"
#include "jobs/job_system.h"
#include "log/logger.h"
#include "physics/physics_system.h"
#include "platform/window.h"

namespace iris
{

Root Root::instance_;

Root::Root()
    : logger_(std::make_unique<Logger>())
    , job_system_(std::make_unique<JobSystem>())
    , physics_system_(std::make_unique<PhysicsSystem>())
    , window_()
    , render_system_()
    , screen_target_()
{
}

void Root::init()
{
    window_ = std::make_unique<Window>(800.0f, 800.0f);
    render_system_ =
        std::make_unique<RenderSystem>(window_->width(), window_->height());
    screen_target_ = std::make_unique<RenderTarget>(
        static_cast<std::uint32_t>(window_->width()),
        static_cast<std::uint32_t>(window_->height()));
}

Root &Root::instance()
{
    return instance_;
}

JobSystem &Root::job_system()
{
    return *instance_.job_system_;
}

Logger &Root::logger()
{
    return *instance_.logger_;
}

PhysicsSystem &Root::physics_system()
{
    return *instance_.physics_system_;
}

RenderSystem &Root::render_system()
{
    return *instance_.render_system_;
}

Window &Root::window()
{
    return *instance_.window_;
}

RenderTarget &Root::screen_target()
{
    return *instance_.screen_target_;
}

}
