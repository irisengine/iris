#include "graphics/material.h"

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/exception.h"
#include "graphics/gl/opengl.h"
#include "graphics/gl/shader.h"
#include "graphics/gl/shader_type.h"

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct Material::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t program)
        : program(program)
    { }

    /** Opengl handle for program. */
    std::uint32_t program;
};


Material::Material(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : impl_(nullptr)
{
    const auto program = ::glCreateProgram();
    check_opengl_error("could not create new program");

    const shader vertex_shader{ vertex_shader_source, shader_type::VERTEX };
    const shader fragment_shader{ fragment_shader_source, shader_type::FRAGMENT };

    ::glAttachShader(program, vertex_shader.native_handle());
    check_opengl_error("could not attach vertex shader");

    ::glAttachShader(program, fragment_shader.native_handle());
    check_opengl_error("could not attach fragment shader");

    ::glLinkProgram(program);

    std::int32_t programparam = 0;

    ::glGetProgramiv(program, GL_LINK_STATUS, &programparam);

    // if program failed to link then get the opengl error
    if(programparam != GL_TRUE)
    {
        ::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programparam);
        eng::check_opengl_error("could not get program log length");

        if(programparam == 0)
        {
            throw Exception("program link failed: no log");
        }
        else
        {
            std::vector<char> error_log(programparam);

            // get opengl error log
            std::int32_t log_length = 0;
            ::glGetProgramInfoLog(
                program,
                static_cast<std::int32_t>(error_log.size()),
                &log_length,
                error_log.data());
            eng::check_opengl_error("failed to get error log");

            const std::string error(error_log.data(), log_length);
            throw Exception("program link failed: " + error);
        }
    }

    impl_ = std::make_unique<implementation>(program);
}

Material::~Material()
{
    ::glDeleteProgram(impl_->program);
}

/** Default */
Material::Material(Material &&other) = default;
Material& Material::operator=(Material &&other) = default;

std::any Material::native_handle() const
{
    return std::any { impl_->program };
}

}

