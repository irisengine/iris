////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "core/data_buffer.h"
#include "graphics/sampler.h"
#include "graphics/texture_usage.h"

namespace iris
{

/**
 * Abstract class that encapsulates a renderable texture.
 */
class Texture
{
  public:
    /**
     * Creates a new Texture with custom data.
     *
     * @param data
     *   Raw data of image, in pixel_format.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param sampler
     *   Sampler to use for this texture.
     *
     * @param usage
     *   Texture usage.
     *
     * @param index
     *   Index into the global array of all allocated textures.
     */
    Texture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler,
        TextureUsage usage,
        std::uint32_t index);

    virtual ~Texture() = 0;

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    /**
     * Get the raw image data.
     *
     * @returns
     *   Raw image data.
     */
    const DataBuffer &data() const;

    /**
     * Get the width of the image.
     *
     * @returns
     *   Image width.
     */
    std::uint32_t width() const;

    /**
     * Get the height of the image.
     *
     * @returns
     *   Image height.
     */
    std::uint32_t height() const;

    /**
     * Get texture sampler.
     *
     * @returns
     *   Sampler for texture.
     */
    const Sampler *sampler() const;

    /**
     * Get the texture usage.
     *
     * @returns
     *   Texture usage.
     */
    TextureUsage usage() const;

    /**
     * Should a texture be flipped vertically.
     *
     * @returns
     *   Should flip.
     */
    bool flip() const;

    /**
     * Set whether texture should be flipped vertically.
     *
     * @param flip
     *   New flip value.
     */
    void set_flip(bool flip);

    /**
     * Get index into global array of all allocated textures.
     *
     * This is most useful for bindless texturing as the texture knows where in the mapped texture table to find itself.
     *
     * @returns
     *   Index of texture.
     */
    std::uint32_t index() const;

  protected:
    /** Raw image data. */
    DataBuffer data_;

    /** Image width. */
    std::uint32_t width_;

    /** Image height. */
    std::uint32_t height_;

    /** Sampler for texture. */
    const Sampler *sampler_;

    /** Should texture be flipped vertically. */
    bool flip_;

    /** Texture usage. */
    TextureUsage usage_;

    /** Index into the global array of all allocated textures. */
    std::uint32_t index_;
};

}
