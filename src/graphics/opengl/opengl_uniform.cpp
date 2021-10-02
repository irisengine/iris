#include "graphics/opengl/opengl_uniform.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "core/exception.h"
#include "core/matrix4.h"
#include "graphics/opengl/opengl.h"

namespace iris
{

OpenGLUniform::OpenGLUniform(
    GLuint program,
    const std::string &name,
    bool ensure_exists)
    : location_(-1)
{
    location_ = ::glGetUniformLocation(program, name.c_str());
    check_opengl_error("could not get uniform location");

    if (ensure_exists && (location_ == -1))
    {
        throw Exception("uniform location does not exist");
    }
}

void OpenGLUniform::set_value(const Matrix4 &value) const
{
    ::glUniformMatrix4fv(
        location_, 1, GL_TRUE, reinterpret_cast<const float *>(value.data()));
    iris::check_opengl_error("could not set uniform data");
}

void OpenGLUniform::set_value(const std::vector<Matrix4> &value) const
{
    ::glUniformMatrix4fv(
        location_,
        static_cast<GLsizei>(value.size()),
        GL_TRUE,
        reinterpret_cast<const float *>(value.data()));
    iris::check_opengl_error("could not set uniform data");
}

void OpenGLUniform::set_value(const std::array<float, 4u> &value) const
{
    ::glUniform4fv(location_, 1u, value.data());
    iris::check_opengl_error("could not set uniform data");
}

void OpenGLUniform::set_value(const std::array<float, 3u> &value) const
{
    ::glUniform1fv(location_, 3u, value.data());
    iris::check_opengl_error("could not set uniform data");
}

void OpenGLUniform::set_value(std::int32_t value) const
{
    ::glUniform1i(location_, value);
    iris::check_opengl_error("could not set uniform data");
}

}
