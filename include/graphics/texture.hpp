#pragma once

#include <any>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

namespace eng
{

/**
 * Class encapsulating a renderable texture. This class loads image data from
 * a file and performs all necessary graphics API setup for use.
 */
class texture final
{
    public:

        /**
         * Create a new texture from an image file.
         *
         * @param path
         *   Path to image file to load.
         */
        explicit texture(const std::filesystem::path &path);

        /**
         * Creates a new texture with custom values.
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
        texture(
            const std::vector<std::uint8_t> &data,
            const std::uint32_t width,
            const std::uint32_t height,
            const std::uint32_t num_channels);

        /** Declared in mm/cpp file as implementation is an incomplete type. */
        ~texture();
        texture(texture&&);
        texture& operator=(texture&&);

        /**
         * Get the raw image data.
         *
         * @returns
         *   Raw image data.
         */
        std::vector<std::uint8_t> data() const noexcept;

        /**
         * Get the width of the image.
         *
         * @returns
         *   Image width.
         */
        std::uint32_t width() const noexcept;

        /**
         * Get the height of the image.
         *
         * @returns
         *   Image height.
         */
        std::uint32_t height() const noexcept;

        /**
         * Get the number of channels in the image.
         *
         * @returns
         *   Number of channels.
         */
        std::uint32_t num_channels() const noexcept;

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
        static texture blank();

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

