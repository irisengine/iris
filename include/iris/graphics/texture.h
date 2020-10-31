#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

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
     * Creates a new Texture with custom values.
     *
     * @param data
     *   Raw data of image.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param num_channels
     *   Number of channels.
     */
    Texture(
        const std::vector<std::uint8_t> &data,
        const std::uint32_t width,
        const std::uint32_t height,
        const std::uint32_t num_channels);

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
    std::vector<std::uint8_t> data() const;

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
     * Get the number of channels in the image.
     *
     * @returns
     *   Number of channels.
     */
    std::uint32_t num_channels() const;

    /**
     * Get a native handle for the texture. The type of this is dependant
     * on the current graphics API.
     *
     * @returns
     *   Graphics API specific handle.
     */
    std::any native_handle() const;

    /**
     * Return a 1x1 pixel white texture.
     *
     * @returns
     *   Blank texture.
     */
    static Texture blank();

  private:
    /** Raw image data. */
    std::vector<std::uint8_t> data_;

    /** Image width. */
    std::uint32_t width_;

    /** Image height. */
    std::uint32_t height_;

    /** Number of channels in image. */
    std::uint32_t num_channels_;

    /** Graphics API implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
