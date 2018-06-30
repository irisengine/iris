#include "gl/material_implementation.hpp"

#include <any>
#include <cstdint>
#include <string>
#include <vector>

#include "exception.hpp"
#include "gl/opengl.hpp"
#include "gl/shader.hpp"
#include "gl/shader_type.hpp"

namespace eng
{

material_implementation::material_implementation(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : program_(0u)
{
    program_ = ::glCreateProgram();
    check_opengl_error("could not create new program");

    const shader vertex_shader{ vertex_shader_source, shader_type::VERTEX };
    const shader fragment_shader{ fragment_shader_source, shader_type::FRAGMENT };

    ::glAttachShader(program_, vertex_shader.native_handle());
    check_opengl_error("could not attach vertex shader");

    ::glAttachShader(program_, fragment_shader.native_handle());
    check_opengl_error("could not attach fragment shader");

    ::glLinkProgram(program_);

    std::int32_t program_param = 0;

    ::glGetProgramiv(program_, GL_LINK_STATUS, &program_param);

    // if program failed to link then get the opengl error
    if(program_param != GL_TRUE)
    {
        ::glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &program_param);
        eng::check_opengl_error("could not get program log length");

        if(program_param == 0)
        {
            throw exception("program link failed: no log");
        }
        else
        {
            std::vector<char> error_log(program_param);

            // get opengl error log
            std::int32_t log_length = 0;
            ::glGetProgramInfoLog(
                program_,
                static_cast<std::int32_t>(error_log.size()),
                &log_length,
                error_log.data());
            eng::check_opengl_error("failed to get error log");

            const std::string error(error_log.data(), log_length);
            throw exception("program link failed: " + error);
        }
    }
}

material_implementation::~material_implementation()
{
    ::glDeleteProgram(program_);
}

material_implementation::material_implementation(material_implementation &&other) noexcept
    : program_(0u)
{
    std::swap(program_, other.program_);
}

material_implementation& material_implementation::operator=(material_implementation &&other) noexcept
{
    // create a new material_implementation object to 'steal' the internal state of the supplied
    // object then swap
    // this ensures that the current material_implementation is correctly deleted at the end
    // of this call
    material_implementation new_material{ std::move(other) };
    std::swap(program_, new_material.program_);

    return *this;
}

std::uint32_t material_implementation::native_handle() const noexcept
{
    return program_;
}

}

