#pragma once

#include <cstdint>
#include <vector>

namespace eng::gl
{

/**
 * This class is the opengl implementation of texture. This encapsulates all the
 * logic needed for opengl to render a texture.
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
         * @param wifth
         *   Width of the texture.
         *
         * @param height
         *   Height of the texture.
         *
         * @param num_channles.
         *   The number of colour channels in the image, this represents the
         *   number of components per pixel in the supplied data.
         */
        texture_implementation(
            const std::vector<std::uint8_t> &data,
            const std::uint32_t width,
            const std::uint32_t height,
            const std::uint32_t num_channels);

        /**
         * Perform opengl cleanup.
         */
        ~texture_implementation();

        /** Default */
        texture_implementation(texture_implementation&&) = default;
        texture_implementation& operator=(texture_implementation&&) = default;

        /** Disabled */
        texture_implementation(const texture_implementation&) = delete;
        texture_implementation& operator=(const texture_implementation&) = delete;

        /**
         * Performs all opengl actions required to be able to use the texture.
         */
        void bind() const;

        /**
         * Performs all opengl actions needed after using the texture.
         */
        void unbind() const;

    private:

        /**
         * Opengl handle to texture.
         */
        std::uint32_t handle_;
};

}

