////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <deque>
#include <memory>
#include <optional>

#include "events/event.h"
#include "graphics/render_pass.h"
#include "graphics/render_pipeline.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"

namespace iris
{

/**
 * Abstract class for a native window.
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
    Window(std::uint32_t width, std::uint32_t height);

    virtual ~Window() = default;

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
    virtual std::optional<Event> pump_event() = 0;

    /**
     * Render the current scene.
     */
    virtual void render() const;

    /**
     * Get the natural scale for the screen. This value reflects the scale
     * factor needed to convert from the default logical coordinate space into
     * the device coordinate space of this screen.
     *
     * @returns
     *   Screen scale factor.
     */
    virtual std::uint32_t screen_scale() const = 0;

    /**
     * Get the width of the window.
     *
     * @returns
     *   Window width.
     */
    std::uint32_t width() const;

    /**
     * Get the height of the window.
     *
     * @returns
     *   Window height.
     */
    std::uint32_t height() const;

    /**
     * Set the render pipeline to execute with render().
     *
     * @param render_pipeline
     *   Pipeline to execute.
     */
    void set_render_pipeline(std::unique_ptr<RenderPipeline> render_pipeline);

  protected:
    /** Window width. */
    std::uint32_t width_;

    /** Window height. */
    std::uint32_t height_;

    /** Renderer to use. */
    std::unique_ptr<Renderer> renderer_;
};

}
