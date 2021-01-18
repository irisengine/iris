#include "graphics/texture.h"

#include <any>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/resource_loader.h"
#include "graphics/opengl/opengl.h"
#include "graphics/pixel_format.h"
#include "log/log.h"

namespace
{

/**
 * Helper method to convert engine pixel format into an opengl enum.
 *
 * @param pixel_format
 *   Format of texture data.
 *
 * @returns
 *   An opengl enum representing the pixel format.
 */
std::uint32_t format_to_opengl(iris::PixelFormat pixel_format)
{
    auto opengl_format = 0u;

    switch (pixel_format)
    {
        case iris::PixelFormat::R: opengl_format = GL_RED; break;
        case iris::PixelFormat::RGB: opengl_format = GL_RGB; break;
        case iris::PixelFormat::RGBA: opengl_format = GL_RGBA; break;
        case iris::PixelFormat::DEPTH:
            opengl_format = GL_DEPTH_COMPONENT;
            break;
        default: throw std::runtime_error("incorrect number of channels");
    }

    return opengl_format;
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
 * @param pixel_format
 *   Format of texture data.
 *
 * @returns
 *   Handle to texture.
 */
std::tuple<std::uint32_t, std::uint32_t> create_texture(
    const iris::DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    iris::PixelFormat pixel_format)
{
    auto texture = 0u;

    static std::uint32_t counter = 0u;

    ::glGenTextures(1, &texture);
    iris::check_opengl_error("could not generate texture");

    ::glActiveTexture(GL_TEXTURE0 + counter);
    iris::check_opengl_error("could not activate texture");

    ::glBindTexture(GL_TEXTURE_2D, texture);
    iris::check_opengl_error("could not bind texture");

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    ::glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    iris::check_opengl_error("could not set min filter parameter");

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    iris::check_opengl_error("could not set max filter parameter");

    const auto format = format_to_opengl(pixel_format);

    // opengl requires a nullptr if there is no data
    const auto *data_ptr = (data.empty()) ? nullptr : data.data();

    const auto type =
        (format == GL_DEPTH_COMPONENT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE;

    // create opengl texture
    ::glTexImage2D(
        GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data_ptr);
    iris::check_opengl_error("could not set Texture data");

    if (format != GL_DEPTH_COMPONENT)
    {
        ::glGenerateMipmap(GL_TEXTURE_2D);
        iris::check_opengl_error("could not generate mipmaps");
    }

    return {texture, counter++};
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Texture::implementation
{
    std::uint32_t texture;
    std::uint32_t id;
};

Texture::Texture()
    : Texture(
          DataBuffer(4, static_cast<std::byte>(0xFF)),
          1u,
          1u,
          PixelFormat::RGBA)
{
    static constexpr auto value = static_cast<std::byte>(0xFF);
    data_ = DataBuffer(4, value);
}

Texture::Texture(
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
    : Texture({}, width, height, pixel_format)
{
}

Texture::Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
    : data_(data)
    , width_(width)
    , height_(height)
    , flip_(false)
    , impl_(std::make_unique<implementation>())
{
    const auto [texture, id] =
        create_texture(data, width, height, pixel_format);
    impl_->texture = texture;
    impl_->id = id;

    LOG_ENGINE_INFO("texture", "loaded from data");
}

Texture::~Texture()
{
    // cleanup opengl resources
    ::glDeleteTextures(1, std::addressof(impl_->texture));
}

/** Default. */
Texture::Texture(Texture &&) = default;
Texture &Texture::operator=(Texture &&) = default;

DataBuffer Texture::data() const
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

std::any Texture::native_handle() const
{
    return impl_->texture;
}

std::uint32_t Texture::texture_id() const
{
    return impl_->id;
}

bool Texture::flip() const
{
    return flip_;
}

void Texture::set_flip(bool flip)
{
    flip_ = flip;
}

}
