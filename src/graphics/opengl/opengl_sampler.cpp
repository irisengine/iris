////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_sampler.h"

#include <cstdint>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"

namespace
{

/**
 * Helper function to convert an engine address mode to an OpenGL address mode.
 *
 * @param address_mode
 *   Engine address mode.
 *
 * @returns
 *   OpenGL address mode.
 */
GLint to_opengl(iris::SamplerAddressMode address_mode)
{
    switch (address_mode)
    {
        case iris::SamplerAddressMode::REPEAT: return GL_REPEAT;
        case iris::SamplerAddressMode::MIRROR: return GL_MIRRORED_REPEAT;
        case iris::SamplerAddressMode::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case iris::SamplerAddressMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
        default: throw iris::Exception("unknown address mode");
    }
}

/**
 * Helper method to convert an engine filter to an OpenGL filter.
 *
 * @param filter
 *   Engine filter.
 *
 * @returns
 *   OpenGL filter.
 */
GLint to_opengl(iris::SamplerFilter filter)
{
    switch (filter)
    {
        case iris::SamplerFilter::NEAREST: return GL_NEAREST;
        case iris::SamplerFilter::LINEAR: return GL_LINEAR;
        default: throw iris::Exception("unknown filter");
    }
}

}

namespace iris
{

OpenGLSampler::OpenGLSampler(const SamplerDescriptor &descriptor, std::uint32_t index)
    : Sampler(descriptor, index)
    , handle_(0u)
{
    ::glGenSamplers(1u, &handle_);
    ensure(check_opengl_error, "could not generate sampler");

    ::glSamplerParameteri(handle_, GL_TEXTURE_WRAP_S, to_opengl(descriptor.s_address_mode));
    ensure(check_opengl_error, "could not set s wrap mode");

    ::glSamplerParameteri(handle_, GL_TEXTURE_WRAP_T, to_opengl(descriptor.t_address_mode));
    ensure(check_opengl_error, "could not set t wrap mode");

    ::glSamplerParameteri(handle_, GL_TEXTURE_WRAP_R, to_opengl(descriptor.r_address_mode));
    ensure(check_opengl_error, "could not set r wrap mode");

    ::glSamplerParameterfv(
        handle_, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float *>(&descriptor.border_colour));

    ::glSamplerParameteri(handle_, GL_TEXTURE_MAG_FILTER, to_opengl(descriptor.magnification_filter));
    ensure(iris::check_opengl_error, "could not set max filter");

    // set mip-specific minification filters
    if (descriptor.uses_mips)
    {
        if ((descriptor.minification_filter == SamplerFilter::NEAREST) &&
            (descriptor.mip_filter == SamplerFilter::NEAREST))
        {
            ::glSamplerParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            ensure(iris::check_opengl_error, "could not set min filter");
        }
        else if (
            (descriptor.minification_filter == SamplerFilter::LINEAR) &&
            (descriptor.mip_filter == SamplerFilter::NEAREST))
        {
            ::glSamplerParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            ensure(iris::check_opengl_error, "could not set min filter");
        }
        else if (
            (descriptor.minification_filter == SamplerFilter::NEAREST) &&
            (descriptor.mip_filter == SamplerFilter::LINEAR))
        {
            ::glSamplerParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            ensure(iris::check_opengl_error, "could not set min filter");
        }
        else if (
            (descriptor.minification_filter == SamplerFilter::LINEAR) &&
            (descriptor.mip_filter == SamplerFilter::LINEAR))
        {
            ::glSamplerParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            ensure(iris::check_opengl_error, "could not set min filter");
        }
        else
        {
            throw Exception("unknown min filter");
        }
    }
    else
    {
        ::glSamplerParameteri(handle_, GL_TEXTURE_MIN_FILTER, to_opengl(descriptor.minification_filter));
        ensure(iris::check_opengl_error, "could not set min filter");
    }
}

OpenGLSampler::~OpenGLSampler()
{
    ::glDeleteSamplers(1u, &handle_);
}

GLuint OpenGLSampler::handle() const
{
    return handle_;
}

};
