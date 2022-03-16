#include "graphics/opengl/opengl_cube_map.h"

#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "graphics/cube_map.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_sampler.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/sampler.h"
#include "log/log.h"

namespace iris
{

OpenGLCubeMap::OpenGLCubeMap(
    const DataBuffer &right_data,
    const DataBuffer &left_data,
    const DataBuffer &top_data,
    const DataBuffer &bottom_data,
    const DataBuffer &back_data,
    const DataBuffer &front_data,
    std::uint32_t width,
    std::uint32_t height,
    const Sampler *sampler,
    std::uint32_t index,
    GLuint id)
    : CubeMap(sampler, index)
    , handle_(0u)
    , id_(id)
{
    const auto *opengl_sampler = static_cast<const iris::OpenGLSampler *>(sampler);

    ::glGenTextures(1u, &handle_);
    expect(check_opengl_error, "could not generate texture");

    ::glActiveTexture(id_);
    expect(check_opengl_error, "could not activate texture");

    ::glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
    expect(check_opengl_error, "could not bind texture");

    ::glBindSampler(id - GL_TEXTURE0, opengl_sampler->handle());
    iris::ensure(iris::check_opengl_error, "could not bind sampler");

    const std::byte *data_ptrs[] = {
        right_data.data(), left_data.data(), top_data.data(), bottom_data.data(), back_data.data(), front_data.data()};

    // specify data for each cube face
    for (auto i = 0u; i < 6u; ++i)
    {
        ::glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_SRGB_ALPHA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data_ptrs[i]);
        expect(check_opengl_error, "could not specify image texture");
    }

    // create a bindless handle and make it resident
    bindless_handle_ = ::glGetTextureSamplerHandleARB(handle_, opengl_sampler->handle());
    expect(check_opengl_error, "could not create bindless handle");

    ::glMakeTextureHandleResidentARB(bindless_handle_);
    expect(check_opengl_error, "could not make bindless handle resident");

    LOG_ENGINE_INFO("opengl_texture", "loaded from data");
}

OpenGLCubeMap::~OpenGLCubeMap()
{
    ::glMakeTextureHandleNonResidentARB(bindless_handle_);
    ::glDeleteTextures(1, &handle_);
}

GLuint OpenGLCubeMap::handle() const
{
    return handle_;
}

GLuint OpenGLCubeMap::id() const
{
    return id_;
}

GLuint64 OpenGLCubeMap::bindless_handle() const
{
    return bindless_handle_;
}

}
