#pragma once

#include <cstdint>
#include <memory>

#include "graphics/anti_aliasing_level.h"
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
    ~Win32WindowManager() override = default;

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

    Window *create_window(
        std::uint32_t width,
        std::uint32_t height,
        AntiAliasingLevel anti_aliasing_level) override;

    /**
     * Get the currently active window.
     *
     * @returns
     *   Pointer to current window, nullptr if one does not exist.
     */
    Window *current_window() const override;

  private:
    /** Current window .*/
    std::unique_ptr<Window> current_window_;
};

}
