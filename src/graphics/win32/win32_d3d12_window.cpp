#include "graphics/win32/win32_d3d12_window.h"

#include <cstdint>
#include <memory>

#include "graphics/anti_aliasing_level.h"
#include "graphics/d3d12/d3d12_renderer.h"

namespace iris
{

Win32D3D12Window::Win32D3D12Window(
    std::uint32_t width,
    std::uint32_t height,
    AntiAliasingLevel anti_aliasing_level)
    : Win32Window(width, height)
{
    renderer_ = std::make_unique<D3D12Renderer>(
        window_, width_, height_, screen_scale(), anti_aliasing_level);
}

}
