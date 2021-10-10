////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace iris
{

class Window;

/**
 * Interface for a class which creates Window objects.
 */
class WindowManager
{
  public:
    virtual ~WindowManager() = default;

    /**
     * Create a new Window.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    virtual Window *create_window(
        std::uint32_t width,
        std::uint32_t height) = 0;

    /**
     * Get the currently active window.
     *
     * @returns
     *   Pointer to current window, nullptr if one does not exist.
     */
    virtual Window *current_window() const = 0;
};

}
