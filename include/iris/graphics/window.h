////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "events/event.h"
#include "graphics/render_pass.h"
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
     * Create a RenderTarget the same size as the Window.
     *
     * @returns
     *   RenderTarget.
     */
    RenderTarget *create_render_target();

    /**
     * Create a RenderTarget with custom dimensions.
     *
     * @param width
     *   Width of render target.
     *
     * @param height
     *   Height of render target.
     *
     * @returns
     *   RenderTarget.
     */
    RenderTarget *create_render_target(
        std::uint32_t width,
        std::uint32_t height);

    /**
     * Set the render passes. These will be executed when render() is called.
     *
     * @param render_passes
     *   Collection of RenderPass objects to render.
     */
    void set_render_passes(const std::vector<RenderPass> &render_passes);

  protected:
    /** Window width. */
    std::uint32_t width_;

    /** Window height. */
    std::uint32_t height_;

    /** Renderer to use. */
    std::unique_ptr<Renderer> renderer_;
};

}
