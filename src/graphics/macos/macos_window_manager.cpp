#include "graphics/macos/macos_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/exception.h"
#include "graphics/anti_aliasing_level.h"
#include "graphics/macos/macos_window.h"

namespace iris
{

Window *MacosWindowManager::create_window(
    std::uint32_t width,
    std::uint32_t height)
{
    return create_window(width, height, AntiAliasingLevel::NONE);
}

Window *MacosWindowManager::create_window(
    std::uint32_t width,
    std::uint32_t height,
    AntiAliasingLevel anti_aliasing_level)
{
    if (current_window_)
    {
        throw Exception("window already created");
    }

    current_window_ =
        std::make_unique<MacosWindow>(width, height, anti_aliasing_level);
    return current_window_.get();
}

Window *MacosWindowManager::current_window() const
{
    return current_window_.get();
}

}
