#include "gl/mesh_implementation.hpp"

#include <cstdint>
#include <vector>

#include "auto_bind.hpp"
#include "gl/buffer.hpp"
#include "gl/opengl.hpp"
#include "gl/vertex_state.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng
{

mesh_implementation::mesh_implementation(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices)
    : vao_(),
      vbo_(vertices, GL_ARRAY_BUFFER),
      ebo_(indices, GL_ELEMENT_ARRAY_BUFFER)
{
    // bind the vao
    auto_bind<gl::vertex_state> auto_state{ vao_ };

    // ensure both buffers are bound for the vao
    vbo_.bind();
    ebo_.bind();

    // setup attributes
    const auto pos_attribute = 0u;

    ::glEnableVertexAttribArray(pos_attribute);
    check_opengl_error("could not enable position attribute");

    const auto data_size = 12 * sizeof(float);

    ::glVertexAttribPointer(pos_attribute, 3, GL_FLOAT, GL_FALSE, data_size, 0);
    check_opengl_error("could not set position attributes");

    const auto normal_attribute = 1u;

    ::glEnableVertexAttribArray(normal_attribute);
    check_opengl_error("could not enable normal attribute");

    ::glVertexAttribPointer(normal_attribute, 3, GL_FLOAT, GL_FALSE, data_size, reinterpret_cast<void*>(3 * sizeof(float)));
    check_opengl_error("could not set normal attributes");

    const auto colour_attribute = 2u;

    ::glEnableVertexAttribArray(colour_attribute);
    check_opengl_error("could not enable colour attribute");

    ::glVertexAttribPointer(colour_attribute, 3, GL_FLOAT, GL_FALSE, data_size, reinterpret_cast<void*>(6 * sizeof(float)));
    check_opengl_error("could not set colour attributes");

    const auto tex_attribute = 3u;

    ::glEnableVertexAttribArray(tex_attribute);
    check_opengl_error("could not enable tex attribute");

    ::glVertexAttribPointer(tex_attribute, 3, GL_FLOAT, GL_FALSE, data_size, reinterpret_cast<void*>(9 * sizeof(float)));
    check_opengl_error("could not set tex attributes");
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

