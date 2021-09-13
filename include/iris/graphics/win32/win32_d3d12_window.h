#pragma once

#include <cstdint>

#include "graphics/anti_aliasing_level.h"
#include "graphics/win32/win32_window.h"

namespace iris
{

/**
 * Implementation of Win32Window for d3d12.
 */
class Win32D3D12Window : public Win32Window
{
  public:
    /**
     * Create a Win32D3D12Window.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    Win32D3D12Window(
        std::uint32_t width,
        std::uint32_t height,
        AntiAliasingLevel anti_aliasing_level);

    ~Win32D3D12Window() override = default;
};

}
