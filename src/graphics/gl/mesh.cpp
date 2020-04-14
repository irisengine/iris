#include "graphics/mesh.hpp"

#include <any>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "core/exception.hpp"
#include "core/matrix4.hpp"
#include "core/quaternion.hpp"
#include "core/vector3.hpp"
#include "graphics/buffer.hpp"
#include "graphics/buffer_type.hpp"
#include "graphics/gl/opengl.hpp"
#include "graphics/vertex_data.hpp"

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

/**
 * Struct containing implementation specific data.
 */
struct mesh::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t vao)
        : vao(vao)
    { }

    /** Opengl handle for vao. */
    std::uint32_t vao;
};

mesh::mesh(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices,
    texture &&textures)
    : indices_(indices),
      texture_(std::move(textures)),
      vertex_buffer_(std::make_unique<buffer>(vertices, buffer_type::VERTEX_ATTRIBUTES)),
      index_buffer_(std::make_unique<buffer>(indices, buffer_type::VERTEX_INDICES)),
      impl_(std::make_unique<implementation>(0u))
{
    // create vao
    ::glGenVertexArrays(1, std::addressof(impl_->vao));
    check_opengl_error("could not generate vao");

    // bind the vao
    ::glBindVertexArray(impl_->vao);
    check_opengl_error("could not bind vao");

    // ensure both buffers are bound for the vao
    bind_buffer(*vertex_buffer_);
    bind_buffer(*index_buffer_);

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

    // unbind vao
    ::glBindVertexArray(0u);
    check_opengl_error("could not unbind vao");
}

mesh::~mesh()
{
    if(impl_)
    {
        ::glDeleteVertexArrays(1, std::addressof(impl_->vao));
    }
}

mesh::mesh(mesh&&) = default;
mesh& mesh::operator=(mesh&&) = default;

const std::vector<std::uint32_t>& mesh::indices() const
{
    return indices_;
}

const buffer& mesh::vertex_buffer() const
{
    return *vertex_buffer_;
}

const buffer& mesh::index_buffer() const
{
    return *index_buffer_;
}

const texture& mesh::tex() const
{
    return texture_;
}

std::any mesh::native_handle() const
{
    return std::any{ impl_->vao };
}

}

