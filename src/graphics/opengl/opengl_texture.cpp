#include "graphics/opengl/opengl_texture.h"

#include <any>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <stack>
#include <vector>

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "core/resource_loader.h"
#include "graphics/opengl/opengl.h"
#include "graphics/texture_usage.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to specify a texture suitable for the texture usage.
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @param data_ptr
 *   Pointer to image data.
 */
void specify_image_texture(
    std::uint32_t width,
    std::uint32_t height,
    const std::byte *data_ptr)
{
    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_SRGB_ALPHA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data_ptr);
    iris::expect(
        iris::check_opengl_error, "could not set specify image texture");

    ::glGenerateMipmap(GL_TEXTURE_2D);
    iris::expect(iris::check_opengl_error, "could not generate mipmaps");
}

/**
 * Helper function to specify a texture suitable for the data usage.
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @param data_ptr
 *   Pointer to image data.
 */
void specify_data_texture(
    std::uint32_t width,
    std::uint32_t height,
    const std::byte *data_ptr)
{
    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data_ptr);
    iris::expect(
        iris::check_opengl_error, "could not set specify data texture");

    ::glGenerateMipmap(GL_TEXTURE_2D);
}

/**
 * Helper function to specify a texture suitable for the render target usage.
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 */
void specify_render_target_texture(std::uint32_t width, std::uint32_t height)
{
    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA16F,
        width,
        height,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr);
    iris::expect(
        iris::check_opengl_error,
        "could not set specify render target texture");
}

/**
 * Helper function to specify a texture suitable for the depth usage.
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 */
void specify_depth_texture(std::uint32_t width, std::uint32_t height)
{
    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        width,
        height,
        0,
        GL_DEPTH_COMPONENT,
        GL_UNSIGNED_SHORT,
        nullptr);
    iris::expect(
        iris::check_opengl_error, "could not set specify depth texture");
}

/**
 * Helper function to create an opengl Texture from data.
 *
 * @param data
 *   Image data. This should be width * hight of pixel_format tuples.
 *
 * @param width
 *   Width of image.
 *
 * @param height
 *   Height of image.
 *
 * @param usage
 *   Texture usage.
 *
 * @returns
 *   Handle to texture.
 */
GLuint create_texture(
    const iris::DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    iris::TextureUsage usage,
    GLuint id)
{
    auto texture = 0u;

    ::glGenTextures(1, &texture);
    iris::expect(iris::check_opengl_error, "could not generate texture");

    ::glActiveTexture(id);
    iris::expect(iris::check_opengl_error, "could not activate texture");

    ::glBindTexture(GL_TEXTURE_2D, texture);
    iris::expect(iris::check_opengl_error, "could not bind texture");

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    iris::expect(
        iris::check_opengl_error, "could not set min filter parameter");

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    iris::expect(
        iris::check_opengl_error, "could not set max filter parameter");

    const float border_colour[] = {1.0f, 1.0f, 1.0f, 1.0f};
    ::glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
    iris::expect(iris::check_opengl_error, "could not set border colour");

    // opengl requires a nullptr if there is no data
    const auto *data_ptr = (data.empty()) ? nullptr : data.data();

    switch (usage)
    {
        case iris::TextureUsage::IMAGE:
            specify_image_texture(width, height, data_ptr);
            break;
        case iris::TextureUsage::DATA:
            specify_data_texture(width, height, data_ptr);
            break;
        case iris::TextureUsage::RENDER_TARGET:
            specify_render_target_texture(width, height);
            break;
        case iris::TextureUsage::DEPTH:
            specify_depth_texture(width, height);
            break;
        default: throw iris::Exception("unknown usage");
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
    TextureUsage usage,
    GLuint id)
    : Texture(data, width, height, usage)
    , handle_(0u)
    , id_(id)
{
    handle_ = create_texture(data, width, height, usage, id_);

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
