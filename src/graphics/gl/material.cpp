#include "graphics/material.h"

#include <any>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "core/exception.h"
#include "graphics/gl/opengl.h"
#include "graphics/gl/shader.h"
#include "graphics/gl/shader_type.h"
#include "graphics/light.h"
#include "graphics/render_graph/compiler.h"
#include "graphics/render_graph/render_graph.h"

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
std::uint32_t create_program(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
{
    const auto program = ::glCreateProgram();
    iris::check_opengl_error("could not create new program");

    const iris::shader vertex_shader{
        vertex_shader_source, iris::shader_type::VERTEX};
    const iris::shader fragment_shader{
        fragment_shader_source, iris::shader_type::FRAGMENT};

    ::glAttachShader(program, vertex_shader.native_handle());
    iris::check_opengl_error("could not attach vertex shader");

    ::glAttachShader(program, fragment_shader.native_handle());
    iris::check_opengl_error("could not attach fragment shader");

    ::glLinkProgram(program);

    std::int32_t programparam = 0;
    ::glGetProgramiv(program, GL_LINK_STATUS, &programparam);

    // if program failed to link then get the opengl error
    if (programparam != GL_TRUE)
    {
        ::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programparam);
        iris::check_opengl_error("could not get program log length");

        if (programparam == 0)
        {
            throw iris::Exception("program link failed: no log");
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
            iris::check_opengl_error("failed to get error log");

            const std::string error(error_log.data(), log_length);
            throw iris::Exception("program link failed: " + error);
        }
    }

    return program;
}

}

namespace iris
{

struct Material::implementation
{
    std::uint32_t program;
};

Material::Material(
    const RenderGraph &render_graph,
    const Mesh &,
    const std::vector<Light *> &lights)
    : textures_()
    , impl_(std::make_unique<implementation>())
{
    Compiler compiler{render_graph, lights};

    impl_->program =
        create_program(compiler.vertex_shader(), compiler.fragment_shader());
    textures_ = compiler.textures();
}

Material::~Material()
{
    ::glDeleteProgram(impl_->program);
}

Material::Material(Material &&other) = default;
Material &Material::operator=(Material &&other) = default;

std::any Material::native_handle() const
{
    return std::any{impl_->program};
}

std::vector<Texture *> Material::textures() const
{
    return textures_;
}

}
