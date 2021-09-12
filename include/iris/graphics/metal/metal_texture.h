#pragma once

#include <cstdint>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

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
     */
    MetalTexture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format,
        std::uint32_t samples = 1u);

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
