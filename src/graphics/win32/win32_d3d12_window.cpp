////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/win32/win32_d3d12_window.h"

#include <any>
#include <optional>
#include <queue>

#define WIN32_LEAN_AND_MEAN
#include <ShellScalingApi.h>
#include <Windows.h>
#include <Windowsx.h>
#include <hidusage.h>

#include "core/auto_release.h"
#include "core/colour.h"
#include "core/exception.h"
#include "events/event.h"
#include "events/quit_event.h"
#include "graphics/d3d12/d3d12_renderer.h"
#include "graphics/material_manager.h"
#include "graphics/render_target.h"
#include "log/log.h"


#pragma comment(lib, "Shcore.lib")

namespace iris
{

Win32D3D12Window::Win32D3D12Window(
    WindowManager &window_manager,
    TextureManager &texture_manager,
    MaterialManager &material_manager,
    std::uint32_t width,
    std::uint32_t height)
    : Win32Window(width, height)
{
    renderer_ = std::make_unique<D3D12Renderer>(
        window_manager, texture_manager, material_manager, window_, width_, height_, screen_scale());

    ::SetWindowTextA(window_, "iris::d3d12");
}

}
