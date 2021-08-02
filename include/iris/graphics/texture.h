#pragma once

#include <cstdint>

#include "core/data_buffer.h"
#include "graphics/pixel_format.h"

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
     * @param pixel_format
     *   Pixel format.
     */
    Texture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format);

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
     * Get the pixel format.
     *
     * @returns
     *   Pixel format.
     */
    PixelFormat pixel_format() const;

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

  protected:
    /** Raw image data. */
    DataBuffer data_;

    /** Image width. */
    std::uint32_t width_;

    /** Image height. */
    std::uint32_t height_;

    /** Should texture be flipped vertically. */
    bool flip_;

    /** Pixel format. */
    PixelFormat format_;
};

}
