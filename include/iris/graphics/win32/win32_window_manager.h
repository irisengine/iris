////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "graphics/material_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

/**
 * Implementation of WindowManager for win32.
 */
class Win32WindowManager : public WindowManager
{
  public:
    /**
     * @param texture_manager
     *   Texture manager object.
     *
     * @param material_manager
     *   Material manager object.
     *
     * @param graphics_api
     *   The graphics api to create a window for.
     */
    Win32WindowManager(
        TextureManager &texture_manager,
        MaterialManager &material_manager,
        const std::string &graphics_api);

    /**
     * Create a new Window.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    Window *create_window(std::uint32_t width, std::uint32_t height) override;

    /**
     * Get the currently active window.
     *
     * @returns
     *   Pointer to current window, nullptr if one does not exist.
     */
    Window *current_window() const override;

  private:
    /** Texture manager object. */
    TextureManager &texture_manager_;

    /** Material manager object. */
    MaterialManager &material_manager_;

    /** Graphics api to create windows for. */
    std::string graphics_api_;

    /** Current window .*/
    std::unique_ptr<Window> current_window_;
};

}
