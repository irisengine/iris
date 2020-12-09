#pragma once

#include <memory>
#include <optional>
#include <queue>

#include "core/camera.h"
#include "graphics/render_system.h"
#include "platform/event.h"

namespace iris
{

/**
 * Class representing a native window.
 */
class Window
{
  public:
    /**
     * Create and display a new native window.
     *
     * @param width
     *   Width of the window.
     *
     * @param height
     *   Height of the window.
     */
    Window(float with, float height);

    /** Disabled */
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    /**
     * Pump the next user input event. Result will be empty if there are no
     * new events.
     *
     * @returns
     *   Optional event.
     */
    std::optional<Event> pump_event();

    /**
     * Get the width of the window.
     *
     * @returns
     *   Window width.
     */
    float width() const;

    /**
     * Get the height of the window.
     *
     * @returns
     *   Window height.
     */
    float height() const;

  private:
    /** Window width. */
    float width_;

    /** Window height. */
    float height_;
};

}
