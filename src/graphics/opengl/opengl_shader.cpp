////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_shader.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/shader_type.h"
#include "log/log.h"

namespace iris
{

OpenGLShader::OpenGLShader(const std::string &source, ShaderType type)
    : shader_(0u)
{
    const auto native_type = (type == ShaderType::VERTEX) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

    shader_ = ::glCreateShader(native_type);
    expect(check_opengl_error, "could not create vertex shader");

    auto shader_c_str = source.data();

    ::glShaderSource(shader_, 1, &shader_c_str, nullptr);
    expect(check_opengl_error, "could not set shader source");

    ::glCompileShader(shader_);

    GLint shader_param = 0;

    ::glGetShaderiv(shader_, GL_COMPILE_STATUS, &shader_param);
    expect(check_opengl_error, "could not get shader parameter");

    // if shader failed to compile then get the opengl error
    if (shader_param != GL_TRUE)
    {
        ::glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &shader_param);
        expect(check_opengl_error, "could not get shader log length");

        if (shader_param == 0)
        {
            throw Exception("shader compilation failed: no log");
        }
        else
        {
            std::vector<char> error_log(shader_param);

            // get opengl error log
            GLsizei log_length = 0;
            ::glGetShaderInfoLog(shader_, static_cast<std::int32_t>(error_log.size()), &log_length, error_log.data());
            expect(check_opengl_error, "failed to get error log");

            // convert to string and throw
            const std::string error(error_log.data(), log_length);
            LOG_ENGINE_ERROR("opengl_shader", "{}\n{}", source, error);

            throw Exception("shader compilation failed: " + error);
        }
    }
}

OpenGLShader::~OpenGLShader()
{
    ::glDeleteShader(shader_);
}

OpenGLShader::OpenGLShader(OpenGLShader &&other)
    : shader_(0u)
{
    std::swap(shader_, other.shader_);
}

OpenGLShader &OpenGLShader::operator=(OpenGLShader &&other)
{
    // create a new shader object to 'steal' the internal state of the supplied
    // object then swap
    // this ensures that the current shader is correctly deleted at the end
    // of this call
    OpenGLShader new_shader{std::move(other)};
    std::swap(shader_, new_shader.shader_);

    return *this;
}

GLuint OpenGLShader::native_handle() const
{
    return shader_;
}

}
