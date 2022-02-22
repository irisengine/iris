#include "graphics/opengl/opengl_cube_map.h"

#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "graphics/cube_map.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_texture.h"

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
    GLuint id)
    : handle_(0u)
    , id_(id)
{
    ::glGenTextures(1u, &handle_);
    expect(check_opengl_error, "could not generate texture");

    ::glActiveTexture(id_);
    expect(check_opengl_error, "could not activate texture");

    ::glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
    expect(check_opengl_error, "could not bind texture");

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

    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    expect(check_opengl_error, "could not set parameter");

    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    expect(check_opengl_error, "could not set parameter");

    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    expect(check_opengl_error, "could not set parameter");

    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    expect(check_opengl_error, "could not set parameter");

    ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    expect(check_opengl_error, "could not set parameter");
}

OpenGLCubeMap::~OpenGLCubeMap()
{
    // cleanup opengl resources
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

}
