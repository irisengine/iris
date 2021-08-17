#pragma once

#include <cstdint>
#include <memory>

#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

/**
 * Implementation of WindowManager for macos.
 */
class MacosWindowManager : public WindowManager
{
  public:
    ~MacosWindowManager() override = default;

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
    /** Current window .*/
    std::unique_ptr<Window> current_window_;
};

}
