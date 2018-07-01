#pragma once

#include <any>
#include <cstdint>
#include <vector>

namespace eng
{

/**
 * This class is the metal implementation of texture. This encapsulates all the
 * logic needed for metal to render a texture.
 */
class texture_implementation final
{
    public:

        /**
         * Create a new texture_implementation.
         *
         * @param data
         *   Collection of bytes representing the raw image data. Expected
         *   size is width * height * num_channels.
         *
         * @param width
         *   Width of the texture.
         *
         * @param height
         *   Height of the texture.
         *
         * @param num_channels.
         *   The number of colour channels in the image, this represents the
         *   number of components per pixel in the supplied data.
         */
        texture_implementation(
            const std::vector<std::uint8_t> &data,
            const std::uint32_t height,
            const std::uint32_t width,
            const std::uint32_t num_channels);

        ~texture_implementation() = default;

        /** Default */
        texture_implementation(texture_implementation&&) = default;
        texture_implementation& operator=(texture_implementation&&) = default;

        /** Disabled */
        texture_implementation(const texture_implementation&) = delete;
        texture_implementation& operator=(const texture_implementation&) = delete;

        /**
         * Gets the native handle for the metal texture.
         *
         * @returns
         *   Metal texture handle.
         */
        std::any native_handle() const;

    private:

        /** Metal handle to texture. */
        std::any texture_;
};

}

