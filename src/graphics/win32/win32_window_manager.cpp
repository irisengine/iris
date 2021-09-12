#include "graphics/win32/win32_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/exception.h"
#include "core/root.h"
#include "graphics/anti_aliasing_level.h"
#include "graphics/win32/win32_d3d12_window.h"
#include "graphics/win32/win32_opengl_window.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

Window *Win32WindowManager::create_window(
    std::uint32_t width,
    std::uint32_t height)
{
    return create_window(width, height, AntiAliasingLevel::NONE);
}

Window *Win32WindowManager::create_window(
    std::uint32_t width,
    std::uint32_t height,
    AntiAliasingLevel anti_aliasing_level)
{
    // only support onw window at the moment
    if (current_window_)
    {
        throw Exception("window already created");
    }

    const auto graphics_api = Root::graphics_api();

    if (graphics_api == "d3d12")
    {
        current_window_ = std::make_unique<Win32D3D12Window>(width, height);
    }
    else if (graphics_api == "opengl")
    {
        current_window_ = std::make_unique<Win32OpenGLWindow>(
            width, height, anti_aliasing_level);
    }
    else
    {
        throw Exception("unknown graphics api");
    }

    return current_window_.get();
}

Window *Win32WindowManager::current_window() const
{
    return current_window_.get();
}

}
