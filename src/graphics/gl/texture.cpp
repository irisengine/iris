#include "graphics/texture.h"

#include <any>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/exception.h"
#include "graphics/gl/opengl.h"
#include "graphics/utility.h"
#include "log/log.h"
#include "platform/resource_loader.h"

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

    switch (num_channels)
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

/**
 * Helper function to create an opengl Texture from data.
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
 *
 * @returns
 *   Handle to texture.
 */
std::uint32_t create_texture(
    const std::vector<std::uint8_t> &data,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t num_channels)
{
    // sanity check we have enough data
    const auto expected_size = width * height * num_channels;
    if (data.size() != expected_size)
    {
        throw iris::Exception("incorrect data size");
    }

    auto texture = 0u;

    ::glGenTextures(1, &texture);
    iris::check_opengl_error("could not generate texture");

    // use default Texture unit
    ::glActiveTexture(GL_TEXTURE0);
    iris::check_opengl_error("could not activiate texture");

    ::glBindTexture(GL_TEXTURE_2D, texture);
    iris::check_opengl_error("could not bind texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    iris::check_opengl_error("could not set wrap s parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    iris::check_opengl_error("could not set wrap t parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    iris::check_opengl_error("could not set min filter parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    iris::check_opengl_error("could not set max filter parameter");

    const auto format = channels_to_format(num_channels);

    // create opengl texture
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
    iris::check_opengl_error("could not set Texture data");

    ::glGenerateMipmap(GL_TEXTURE_2D);
    iris::check_opengl_error("could not generate mipmaps");

    return texture;
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Texture::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t texture)
        : texture(texture)
    {
    }

    /** Opengl handle for texture. */
    std::uint32_t texture;
};

Texture::Texture(const std::string &resource)
    : data_()
    , width_(0u)
    , height_(0u)
    , num_channels_(0u)
    , impl_(nullptr)
{
    const auto file_data = ResourceLoader::instance().load(resource);
    const auto [data, width, height, num_channels] =
        graphics::utility::parse_image(file_data);

    const auto texture = create_texture(data, width, height, num_channels);
    impl_ = std::make_unique<implementation>(texture);

    LOG_ENGINE_INFO("texture", "loaded from file");
}

Texture::Texture(
    const std::vector<std::uint8_t> &data,
    const std::uint32_t width,
    const std::uint32_t height,
    const std::uint32_t num_channels)
    : data_(data)
    , width_(width)
    , height_(height)
    , num_channels_(num_channels)
    , impl_(nullptr)
{
    const auto texture = create_texture(data, width, height, num_channels);
    impl_ = std::make_unique<implementation>(texture);

    LOG_ENGINE_INFO("texture", "loaded from data");
}

Texture::~Texture()
{
    if (impl_)
    {
        // cleanup opengl resources
        ::glDeleteTextures(1, std::addressof(impl_->texture));
    }
}

/** Default. */
Texture::Texture(Texture &&) = default;
Texture &Texture::operator=(Texture &&) = default;

std::vector<std::uint8_t> Texture::data() const
{
    return data_;
}

std::uint32_t Texture::width() const
{
    return width_;
}

std::uint32_t Texture::height() const
{
    return height_;
}

std::uint32_t Texture::num_channels() const
{
    return num_channels_;
}

std::any Texture::native_handle() const
{
    return impl_->texture;
}

Texture Texture::blank()
{
    return {{0xFF, 0xFF, 0xFF, 0xFF}, 1u, 1u, 4u};
}

}
