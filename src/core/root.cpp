#include "core/root.h"

#include <memory>

#include "core/exception.h"
#include "graphics/render_system.h"
#include "log/logger.h"
#include "physics/physics_system.h"
#include "platform/window.h"

namespace iris
{

Root Root::instance_;

Root::Root()
    : window_(nullptr)
    , screen_target_(nullptr)
{
}

void Root::init()
{
    window_ = std::make_unique<Window>(800.0f, 800.0f);
    screen_target_ = std::make_unique<RenderTarget>(
        static_cast<std::uint32_t>(window_->width()),
        static_cast<std::uint32_t>(window_->height()));
}

Root &Root::instance()
{
    return instance_;
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
