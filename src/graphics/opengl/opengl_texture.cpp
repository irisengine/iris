#include "graphics/opengl/opengl_texture.h"

#include <any>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <stack>
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
 * In order to have a pool of reusable texture ids we maintain a static stack.
 * This will be lazily populated by the first loaded texture, from there any
 * deleted texture will return its id to this pool.
 */
static std::stack<std::uint32_t> id_pool;

/**
 * Helper method to convert engine pixel format into an opengl enum.
 *
 * @param pixel_format
 *   Format of texture data.
 *
 * @returns
 *   An opengl enum representing the pixel format.
 */
GLenum format_to_opengl(iris::PixelFormat pixel_format)
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
 *
 *   Image data. This should be width * hight of pixel_format tuples.
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
GLuint create_texture(
    const iris::DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    iris::PixelFormat pixel_format,
    GLuint id)
{
    auto texture = 0u;

    ::glGenTextures(1, &texture);
    iris::check_opengl_error("could not generate texture");

    ::glActiveTexture(id);
    iris::check_opengl_error("could not activate texture");

    ::glBindTexture(GL_TEXTURE_2D, texture);
    iris::check_opengl_error("could not bind texture");

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    iris::check_opengl_error("could not set min filter parameter");

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    iris::check_opengl_error("could not set max filter parameter");

    const float border_colour[] = {1.0f, 1.0f, 1.0f, 1.0f};
    ::glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
    iris::check_opengl_error("could not set border colour");

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

    return texture;
}

}

namespace iris
{

OpenGLTexture::OpenGLTexture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format,
    GLuint id)
    : Texture(data, width, height, pixel_format)
    , handle_(0u)
    , id_(id)
{
    handle_ = create_texture(data, width, height, pixel_format, id_);

    LOG_ENGINE_INFO("texture", "loaded from data");
}

OpenGLTexture::~OpenGLTexture()
{
    // cleanup opengl resources
    ::glDeleteTextures(1, &handle_);
}

GLuint OpenGLTexture::handle() const
{
    return handle_;
}
GLuint OpenGLTexture::id() const
{
    return id_;
}

}
