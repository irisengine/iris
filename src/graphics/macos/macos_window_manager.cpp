#include "graphics/macos/macos_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "graphics/macos/macos_window.h"

namespace iris
{

Window *MacosWindowManager::create_window(
    std::uint32_t width,
    std::uint32_t height)
{
    ensure(!current_window_, "window already created");

    current_window_ = std::make_unique<MacosWindow>(width, height);
    return current_window_.get();
}

Window *MacosWindowManager::current_window() const
{
    return current_window_.get();
}

}
