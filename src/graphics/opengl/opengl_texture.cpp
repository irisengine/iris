////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
#include "graphics/opengl/opengl_sampler.h"
#include "graphics/sampler.h"
#include "graphics/texture_usage.h"
#include "graphics/utils.h"
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
 * @param data
 *   Image data.
 *
 * @param sampler_descriptor
 *   Sampler description.
 */
void specify_image_texture(
    std::uint32_t width,
    std::uint32_t height,
    const iris::DataBuffer &data,
    const iris::SamplerDescriptor &sampler_descriptor)
{
    if (!sampler_descriptor.uses_mips)
    {
        ::glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        iris::ensure(iris::check_opengl_error, "could not set specify image texture");
    }
    else
    {
        // generate mipmaps and upload each one
        auto level = 0u;
        for (const auto &[mip_data, mip_width, mip_height] :
             iris::generate_mip_maps({.data = data, .width = width, .height = height}))
        {
            ::glTexImage2D(
                GL_TEXTURE_2D,
                level,
                GL_SRGB_ALPHA,
                mip_width,
                mip_height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                mip_data.data());
            iris::ensure(iris::check_opengl_error, "could not set specify image texture");

            ++level;
        }
    }
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
 * @param data
 *   Image data.
 *
 * @param sampler_descriptor
 *   Sampler description.
 */
void specify_data_texture(
    std::uint32_t width,
    std::uint32_t height,
    const iris::DataBuffer &data,
    const iris::SamplerDescriptor &sampler_descriptor)
{
    if (!sampler_descriptor.uses_mips)
    {
        ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        iris::ensure(iris::check_opengl_error, "could not set specify data texture");
    }
    else
    {
        // generate mipmaps and upload each one
        auto level = 0u;
        for (const auto &[mip_data, mip_width, mip_height] :
             iris::generate_mip_maps({.data = data, .width = width, .height = height}))
        {
            ::glTexImage2D(
                GL_TEXTURE_2D, level, GL_RGBA, mip_width, mip_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mip_data.data());
            iris::ensure(iris::check_opengl_error, "could not set specify image texture");

            ++level;
        }
    }
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
    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    iris::ensure(iris::check_opengl_error, "could not set specify render target texture");
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
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
    iris::ensure(iris::check_opengl_error, "could not set specify depth texture");
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
 * @param sampler
 *   Sampler to use for this texture.
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
    const iris::Sampler *sampler,
    iris::TextureUsage usage)
{
    auto texture = 0u;

    ::glGenTextures(1, &texture);
    iris::ensure(iris::check_opengl_error, "could not generate texture");

    ::glBindTexture(GL_TEXTURE_2D, texture);
    iris::ensure(iris::check_opengl_error, "could not bind texture");

    // specify the image data based on the usage type
    switch (usage)
    {
        case iris::TextureUsage::IMAGE: specify_image_texture(width, height, data, sampler->descriptor()); break;
        case iris::TextureUsage::DATA: specify_data_texture(width, height, data, sampler->descriptor()); break;
        case iris::TextureUsage::RENDER_TARGET: specify_render_target_texture(width, height); break;
        case iris::TextureUsage::DEPTH: specify_depth_texture(width, height); break;
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
    const Sampler *sampler,
    TextureUsage usage,
    std::uint32_t index)
    : Texture(data, width, height, sampler, usage, index)
    , handle_(create_texture(data, width, height, sampler, usage))
    , bindless_handle_(0u)
{
    const auto *opengl_sampler = static_cast<const iris::OpenGLSampler *>(sampler);

    // create a bindless handle and make it resident
    bindless_handle_ = ::glGetTextureSamplerHandleARB(handle_, opengl_sampler->handle());
    ensure(check_opengl_error, "could not create bindless handle");

    ::glMakeTextureHandleResidentARB(bindless_handle_);
    ensure(check_opengl_error, "could not make bindless handle resident");

    LOG_ENGINE_INFO("opengl_texture", "loaded from data");
}

OpenGLTexture::~OpenGLTexture()
{
    ::glMakeTextureHandleNonResidentARB(bindless_handle_);
    ::glDeleteTextures(1, &handle_);
}

GLuint OpenGLTexture::handle() const
{
    return handle_;
}

GLuint64 OpenGLTexture::bindless_handle() const
{
    return bindless_handle_;
}

}
