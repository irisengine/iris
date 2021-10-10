////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "graphics/window.h"

namespace iris
{

/**
 * Implementation of Window for macOS.
 */
class MacosWindow : public Window
{
  public:
    /**
     * Construct a new MacosWindow.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    MacosWindow(std::uint32_t width, std::uint32_t height);
    ~MacosWindow() override = default;

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
};

}
