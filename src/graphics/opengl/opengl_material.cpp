////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_material.h"

#include <cstdint>
#include <string>
#include <vector>

#include "core/error_handling.h"
#include "graphics/default_shader_languages.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_shader.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/shader_type.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to create an opengl program.
 *
 * @param vertex_shader_source
 *   Source for vertex shader.
 *
 * @param fragment_shader_source
 *   Source for fragment shader.
 *
 * @returns
 *   Opengl program object.
 */
GLuint create_program(const std::string &vertex_shader_source, const std::string &fragment_shader_source)
{
    const auto program = ::glCreateProgram();
    iris::expect(iris::check_opengl_error, "could not create new program");

    const iris::OpenGLShader vertex_shader{vertex_shader_source, iris::ShaderType::VERTEX};
    const iris::OpenGLShader fragment_shader{fragment_shader_source, iris::ShaderType::FRAGMENT};

    ::glAttachShader(program, vertex_shader.native_handle());
    iris::expect(iris::check_opengl_error, "could not attach vertex shader");

    ::glAttachShader(program, fragment_shader.native_handle());
    iris::expect(iris::check_opengl_error, "could not attach fragment shader");

    ::glLinkProgram(program);

    GLint programparam = 0;
    ::glGetProgramiv(program, GL_LINK_STATUS, &programparam);

    // if program failed to link then get the opengl error
    if (programparam != GL_TRUE)
    {
        ::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programparam);
        iris::expect(iris::check_opengl_error, "could not get program log length");

        if (programparam == 0)
        {
            throw iris::Exception("program link failed: no log");
        }
        else
        {
            std::vector<char> error_log(programparam);

            // get opengl error log
            GLsizei log_length = 0;
            ::glGetProgramInfoLog(program, static_cast<std::int32_t>(error_log.size()), &log_length, error_log.data());
            iris::expect(iris::check_opengl_error, "failed to get error log");

            const std::string error(error_log.data(), log_length);
            LOG_ENGINE_ERROR("opengl_material", "{}\n{}\n{}", vertex_shader_source, fragment_shader_source, error);

            throw iris::Exception("program link failed: " + error);
        }
    }

    return program;
}

}

namespace iris
{

OpenGLMaterial::OpenGLMaterial(
    const RenderGraph *render_graph,
    LightType light_type,
    bool render_to_normal_target,
    bool render_to_position_target)
    : Material(render_graph)
    , handle_(0u)
{
    ShaderCompiler compiler{
        ShaderLanguage::GLSL, render_graph, light_type, render_to_normal_target, render_to_position_target};

    handle_ = create_program(compiler.vertex_shader(), compiler.fragment_shader());
}

OpenGLMaterial::~OpenGLMaterial()
{
    ::glDeleteProgram(handle_);
}

void OpenGLMaterial::bind() const
{
    ::glUseProgram(handle_);
    expect(check_opengl_error, "could not bind program");
}

GLuint OpenGLMaterial::handle() const
{
    return handle_;
}

}
