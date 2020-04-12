#include "texture.hpp"

#include <any>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "exception.hpp"
#include "gl/opengl.hpp"
#include "log.hpp"
#include "utility.hpp"

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


/**
 * Helper function to create an opengl texture from data.
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
    if(data.size() != expected_size)
    {
        throw eng::exception("incorrect data size");
    }

    auto texture = 0u;

    ::glGenTextures(1, &texture);
    eng::check_opengl_error("could not generate texture");

    // use default texture unit
    ::glActiveTexture(GL_TEXTURE0);
    eng::check_opengl_error("could not activiate texture");

    ::glBindTexture(GL_TEXTURE_2D, texture);
    eng::check_opengl_error("could not bind texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    eng::check_opengl_error("could not set wrap s parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    eng::check_opengl_error("could not set wrap t parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    eng::check_opengl_error("could not set min filter parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    eng::check_opengl_error("could not set max filter parameter");

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
    eng::check_opengl_error("could not set texture data");

    ::glGenerateMipmap(GL_TEXTURE_2D);
    eng::check_opengl_error("could not generate mipmaps");

    return texture;
}

}

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct texture::implementation final
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t texture)
        : texture(texture)
    { }

    /** Default */
    implementation() = default;
    ~implementation() = default;
    implementation(const implementation&) = default;
    implementation& operator=(const implementation&) = default;
    implementation(implementation&&) = default;
    implementation& operator=(implementation&&) = default;

    /** Opengl handle for texture. */
    std::uint32_t texture;
};

texture::texture(const std::filesystem::path &path)
    : data_(),
      width_(0u),
      height_(0u),
      num_channels_(0u),
      impl_(nullptr)
{
    const auto [data, width, height, num_channels] =
        graphics::utility::load_image(path);

    const auto texture = create_texture(data, width, height, num_channels);
    impl_ = std::make_unique<implementation>(texture);

    LOG_ENGINE_INFO("texture", "loaded from file");
}

texture::texture(
    const std::vector<std::uint8_t> &data,
    const std::uint32_t width,
    const std::uint32_t height,
    const std::uint32_t num_channels)
    : data_(data),
      width_(width),
      height_(height),
      num_channels_(num_channels),
      impl_(nullptr)
{
    const auto texture = create_texture(data, width, height, num_channels);
    impl_ = std::make_unique<implementation>(texture);

    LOG_ENGINE_INFO("texture", "loaded from data");
}

texture::~texture()
{
    if(impl_)
    {
        // cleanup opengl resources
        ::glDeleteTextures(1, std::addressof(impl_->texture));
    }
}

/** Default. */
texture::texture(texture&&) = default;
texture& texture::operator=(texture&&) = default;

std::vector<std::uint8_t> texture::data() const noexcept
{
    return data_;
}

std::uint32_t texture::width() const noexcept
{
    return width_;
}

std::uint32_t texture::height() const noexcept
{
    return height_;
}

std::uint32_t texture::num_channels() const noexcept
{
    return num_channels_;
}

std::any texture::native_handle() const
{
    return impl_->texture;
}

texture texture::blank()
{
    return{ { 0xFF, 0xFF, 0xFF, 0xFF }, 1u, 1u, 4u };
}

}

