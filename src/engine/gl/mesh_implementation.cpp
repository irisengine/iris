#include "gl/mesh_implementation.hpp"

#include <cstdint>
#include <vector>

#include "gl/buffer.hpp"
#include "gl/opengl.hpp"
#include "gl/vertex_state.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng::gl
{

mesh_implementation::mesh_implementation(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices)
    : vao_(),
      vbo_(vertices, GL_ARRAY_BUFFER),
      ebo_(indices, GL_ELEMENT_ARRAY_BUFFER)
{
    // bind the vao
    gl::auto_bind<gl::vertex_state> auto_state{ vao_ };

    // ensure both buffers are bound for the vao
    vbo_.bind();
    ebo_.bind();

    // setup attributes
    const auto pos_attribute = 0u;

    ::glEnableVertexAttribArray(pos_attribute);
    gl::check_opengl_error("could not enable position attribute");

    ::glVertexAttribPointer(pos_attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    gl::check_opengl_error("could not set position attributes");

    const auto tex_attribute = 1u;

    ::glEnableVertexAttribArray(tex_attribute);
    gl::check_opengl_error("could not enable tex attribute");

    ::glVertexAttribPointer(tex_attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    gl::check_opengl_error("could not set tex attributes");
}

void mesh_implementation::bind() const
{
    vao_.bind();
}

void mesh_implementation::unbind() const
{
    vao_.unbind();
}

}

