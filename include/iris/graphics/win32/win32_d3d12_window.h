////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "graphics/material_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/win32/win32_window.h"
#include "graphics/window_manager.h"

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
     * @param window_manager
     *   Window manager object.
     *
     * @param texture_manager
     *   Texture manager object.
     *
     * @param material_manager
     *   Material manager object.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    Win32D3D12Window(
        WindowManager &window_manager,
        TextureManager &texture_manager,
        MaterialManager &material_manager,
        std::uint32_t width,
        std::uint32_t height);
};

}
