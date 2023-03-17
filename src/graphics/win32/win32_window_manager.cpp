////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/win32/win32_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "graphics/material_manager.h"
#include "graphics/win32/win32_d3d12_window.h"
#include "graphics/win32/win32_opengl_window.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

Win32WindowManager::Win32WindowManager(
    TextureManager &texture_manager,
    MaterialManager &material_manager,
    const std::string &graphics_api)
    : texture_manager_(texture_manager)
    , material_manager_(material_manager)
    , graphics_api_(graphics_api)
    , current_window_()
{
}

Window *Win32WindowManager::create_window(std::uint32_t width, std::uint32_t height)
{
    // only support onw window at the moment
    ensure(!current_window_, "window already created");

    if (graphics_api_ == "d3d12")
    {
        current_window_ = std::make_unique<Win32D3D12Window>(*this, texture_manager_, material_manager_, width, height);
    }
    else if (graphics_api_ == "opengl")
    {
        current_window_ =
            std::make_unique<Win32OpenGLWindow>(*this, texture_manager_, material_manager_, width, height);
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
