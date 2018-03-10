#pragma once

#include <cstdint>
#include <experimental/filesystem>
#include <memory>
#include <vector>

#include "gl/texture_implementation.hpp"

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
        explicit texture(const std::experimental::filesystem::path &path);

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

        /** Default */
        ~texture() = default;
        texture(texture&&) = default;
        texture& operator=(texture&&) = default;

        /** Disabled */
        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        /**
         * Perform all actions needed to use texture for rendering.
         */
        void bind() const;

        /**
         * Perform all actions needed after texture has been rendered.
         */
        void unbind() const;

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

    private:

        /** Raw image data. */
        std::vector<std::uint8_t> data_;

        /** Image width. */
        std::uint32_t width_;

        /** Image height. */
        std::uint32_t height_;

        /** Number of channels in image. */
        std::uint32_t num_channels_;

        /** Graphics API specific implementation. */
        std::unique_ptr<gl::texture_implementation> impl_;
};

}

