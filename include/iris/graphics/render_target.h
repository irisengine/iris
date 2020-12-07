#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include "graphics/texture.h"

namespace iris
{

/**
 * This class represents a target for rendering i.e. something that the renderer
 * can render to. It also provides access to the colour and depth information
 * post render.
 */
class RenderTarget
{
  public:
    /**
     * Create a new RenderTarget.
     *
     * @param width
     *   Width of target in pixels.
     *
     * @param height
     *   Height of target in pixels.
     */
    RenderTarget(std::uint32_t width, std::uint32_t height);

    ~RenderTarget();

    /**
     * Get native handle to the API specific implementation of the render
     * target.
     *
     * @returns
     *   API specific handle.
     */
    std::any native_handle() const;

    /**
     * Get a pointer to the texture storing the target colour data.
     *
     * @returns
     *   Colour texture.
     */
    Texture *colour_texture() const;

    /**
     * Get a pointer to the texture storing the target depth data.
     *
     * @returns
     *   Depth texture.
     */
    Texture *depth_texture() const;

  private:
    /** Colour texture. */
    std::unique_ptr<Texture> colour_texture_;

    /** Depth texture. */
    std::unique_ptr<Texture> depth_texture_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
