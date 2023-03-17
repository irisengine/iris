////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <optional>
#include <queue>

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "core/auto_release.h"
#include "events/event.h"
#include "graphics/material_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

/**
 * Abstract implementation of Window for linux.
 */
class LinuxWindow : public Window
{
  public:
    /**
     * Construct a new LinuxWindow.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     *
     * @param window_manager
     *   Window manager object.
     *
     * @param texture_manager
     *   Texture manager object.
     *
     * @param material_manager
     *   Material manager object.
     */
    LinuxWindow(
        std::uint32_t width,
        std::uint32_t height,
        WindowManager &window_manager,
        TextureManager &texture_manager,
        MaterialManager &material_manager);

    /**
     * Get the natural scale for the screen. This value reflects the scale
     * factor needed to convert from the default logical coordinate space into
     * the device coordinate space of this screen.
     *
     * @returns
     *   Screen scale factor.
     */
    std::uint32_t screen_scale() const override;

    /**
     * Pump the next user input event. Result will be empty if there are no
     * new events.
     *
     * @returns
     *   Optional event.
     */
    std::optional<Event> pump_event() override;

    /**
     * Get display handle.
     *
     * @returns
     *   Handle to X11 display.
     */
    Display *display() const;

    /**
     * Get window handle.
     *
     * @returns
     *   Handle to X11 window.
     */
    ::Window window() const;

  private:
    /** X11 display handle. */
    AutoRelease<Display *, nullptr> display_;

    /** X11 window handle. */
    AutoRelease<::Window, 0> window_;

    /** OpenGL context object. */
    AutoRelease<GLXContext, nullptr> context_;

    /** Queue of input events. */
    std::queue<Event> events_;
};

}
