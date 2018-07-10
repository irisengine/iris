#include "gl/mesh_implementation.hpp"

#include <any>
#include <cstdint>
#include <vector>

#include "auto_bind.hpp"
#include "buffer.hpp"
#include "buffer_type.hpp"
#include "exception.hpp"
#include "gl/opengl.hpp"
#include "gl/vertex_state.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace
{

/**
 * Helper function to convert internal buffer type to a opengl type.
 *
 * @param type
 *   Type to convert.
 *
 * @returns
 *   Supplied type as opengl type.
 */
std::uint32_t type_to_gl_type(const eng::buffer_type type)
{
    auto gl_type = GL_ARRAY_BUFFER;

    switch(type)
    {
        case eng::buffer_type::VERTEX_ATTRIBUTES:
            gl_type = GL_ARRAY_BUFFER;
            break;
        case eng::buffer_type::VERTEX_INDICES:
            gl_type = GL_ELEMENT_ARRAY_BUFFER;
            break;
        default:
            throw eng::exception("unknown buffer type");
    }

    return gl_type;
}

/**
 * Helper function to bind an opengl buffer.
 *
 * @param buffer
 *   Buffer to bind.
 */
void bind_buffer(const eng::buffer &buffer)
{
    const auto handle = std::any_cast<std::uint32_t>(buffer.native_handle());

    ::glBindBuffer(type_to_gl_type(buffer.type()), handle);
    eng::check_opengl_error("could not bind buffer");
}

}

namespace eng
{

mesh_implementation::mesh_implementation(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices)
    : vao_(),
      vbo_(vertices, buffer_type::VERTEX_ATTRIBUTES),
      ebo_(indices, buffer_type::VERTEX_INDICES)
{
    // bind the vao
    auto_bind<vertex_state> auto_state{ vao_ };

    // ensure both buffers are bound for the vao
    bind_buffer(vbo_);
    bind_buffer(ebo_);

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

const buffer& mesh_implementation::vertex_buffer() const noexcept
{
    return vbo_;
}

const buffer& mesh_implementation::index_buffer() const noexcept
{
    return ebo_;
}

std::any mesh_implementation::native_handle() const
{
    return std::any{ &vao_ };
}

}

