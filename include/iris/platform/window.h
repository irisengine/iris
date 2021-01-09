#pragma once

#include <memory>
#include <optional>
#include <queue>

#include "core/camera.h"
#include "graphics/pipeline.h"
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

    ~Window();

    /** Disabled */
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    /**
     * Render a Pipeline. Afterwards the default screen target is displayed.
     *
     * @param pipeline
     *   Pipeline to execute.
     */
    void render(const Pipeline &pipeline) const;

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

    /** Render system for window. */
    std::unique_ptr<RenderSystem> render_system_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
