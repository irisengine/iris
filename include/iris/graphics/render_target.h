////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>

#include "graphics/texture.h"

namespace iris
{

/**
 * Abstract class for a RenderTarget - a class that encapsulates something the
 * renderer can render to. It also provides access to the colour and depth
 * Texture objects.
 *
 * Note that you cannot access the colour or depth data directly after a render
 * as it is not synchronised back to the CPU, you can use those textures as
 * inputs for other rendering operations.
 */
class RenderTarget
{
  public:
    /**
     * Create a new RenderTarget.
     *
     * @param colour_texture
     *   Texture to render colour data to.
     *
     * @param depth_texture
     *   Texture to render depth data to.
     */
    RenderTarget(std::unique_ptr<Texture> colour_texture, std::unique_ptr<Texture> depth_texture);

    virtual ~RenderTarget() = 0;

    /**
     * Get a pointer to the texture storing the target colour data.
     *
     * @returns
     *   Colour texture.
     */
    const Texture *colour_texture() const;

    /**
     * Get a pointer to the texture storing the target depth data.
     *
     * @returns
     *   Depth texture.
     */
    const Texture *depth_texture() const;

    /**
     * Get the width of the RenderTarget.
     *
     * @returns
     *   Render target width.
     */
    std::uint32_t width() const;

    /**
     * Get the height of the RenderTarget.
     *
     * @returns
     *   Render target height.
     */
    std::uint32_t height() const;

  protected:
    /** Colour texture. */
    std::unique_ptr<Texture> colour_texture_;

    /** Depth texture. */
    std::unique_ptr<Texture> depth_texture_;
};

}
