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
