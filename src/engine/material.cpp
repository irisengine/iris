#include "material.hpp"

#include <string>
#include <utility>
#include <vector>

#include "gl/material_implementation.hpp"
#include "gl/opengl.hpp"
#include "gl/shader.hpp"

namespace eng
{

material::material(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : impl_(vertex_shader_source, fragment_shader_source)
{ }

void material::bind() const
{
    impl_.bind();
}

void material::unbind() const
{
    impl_.unbind();
}

}

