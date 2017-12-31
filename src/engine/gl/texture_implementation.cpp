#include "gl/texture_implementation.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "auto_bind.hpp"
#include "gl/opengl.hpp"

namespace
{

/**
 * Helper method to convert number of channels into an opengl enum.
 *
 * @param num_channels
 *   The number of channels in an image.
 *
 * @returns
 *   An opengl enum representing the number of channels.
 */
std::uint32_t channels_to_format(const std::uint32_t num_channels)
{
    auto format = 0u;

    switch(num_channels)
    {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            throw std::runtime_error("incorrect number of channels");
    }

    return format;
}

}

namespace eng::gl
{

texture_implementation::texture_implementation(
    const std::vector<std::uint8_t> &data,
    const std::uint32_t width,
    const std::uint32_t height,
    const std::uint32_t num_channels)
    : handle_(0u)
{
    ::glGenTextures(1, &handle_);
    gl::check_opengl_error("could not generate texture");

    auto_bind<texture_implementation> auto_bind{ *this };

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl::check_opengl_error("could not set wrap s parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl::check_opengl_error("could not set wrap t parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::check_opengl_error("could not set min filter parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::check_opengl_error("could not set max filter parameter");

    const auto format = channels_to_format(num_channels);

    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data.data());
    gl::check_opengl_error("could not set texture data");

    ::glGenerateMipmap(GL_TEXTURE_2D);
    gl::check_opengl_error("could not generate mipmaps");
}

texture_implementation::~texture_implementation()
{
    ::glDeleteTextures(1, &handle_);
}

void texture_implementation::bind() const
{
    // use default texture unit
    ::glActiveTexture(GL_TEXTURE0);
    gl::check_opengl_error("could not activiate texture");

    ::glBindTexture(GL_TEXTURE_2D, handle_);
    gl::check_opengl_error("could not bind texture");
}

void texture_implementation::unbind() const
{
    // nothing to be done for texture unbinding
}

}

