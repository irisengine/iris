#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/data_buffer.h"
#include "graphics/pixel_format.h"

namespace iris
{

/**
 * Class encapsulating a renderable texture. This class parses image data and
 * performs all necessary graphics API setup for use.
 */
class Texture
{
  public:
    /**
     * Creates a new empty Texture.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param pixel_format
     *   Number of channels.
     */
    Texture(
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format);

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
     *   Number of channels.
     */
    Texture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format);

    /** Declared in mm/cpp file as implementation is an incomplete type. */
    ~Texture();
    Texture(Texture &&);
    Texture &operator=(Texture &&);

    /**
     * Get the raw image data.
     *
     * @returns
     *   Raw image data.
     */
    DataBuffer data() const;

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
     * Get a native handle for the texture. The type of this is dependant
     * on the current graphics API.
     *
     * @returns
     *   Graphics API specific handle.
     */
    std::any native_handle() const;

    /**
     * Get a unique id for this texture.
     *
     * For most APIs this is just a unique integer (starting at 0 and
     * increasing for each subsequent texture). For opengl it is the texture
     * unit i.e. GL_TEXTURE0 + texture_id.
     *
     * @returns
     *   Unique id.
     */
    std::uint32_t texture_id() const;

    /**
     * Return a 1x1 pixel white texture.
     *
     * @returns
     *   Blank texture.
     */
    static Texture blank();

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

  private:
    /** Raw image data. */
    DataBuffer data_;

    /** Image width. */
    std::uint32_t width_;

    /** Image height. */
    std::uint32_t height_;

    /** Should texture be flipped vertically. */
    bool flip_;

    /** Graphics API implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
