////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "graphics/texture.h"
#include "graphics/texture_usage.h"

namespace iris
{

/**
 * Implementation of Texture for metal.
 */
class MetalTexture : public Texture
{
  public:
    /**
     * Construct a new MetalTexture.
     *
     * @param data
     *   Image data. This should be width * hight of pixel_format tuples.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of data.
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
    MetalTexture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler,
        TextureUsage usage,
        std::uint32_t index);

    ~MetalTexture() override = default;

    /**
     * Get metal handle to texture.
     *
     * @returns
     *   Metal texture handle.
     */
    id<MTLTexture> handle() const;

  private:
    /** Metal texture handle. */
    id<MTLTexture> texture_;
};

}
