////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_mesh.h"

#include <memory>
#include <tuple>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/vertex_attributes.h"

namespace
{

/**
 * Convert an engine VertexAttributeType to an OpenGL data type.
 *
 * @param type
 *   Type to convert
 *
 * @returns
 *   Tuple of OpenGL type and boolean indicating if the returned type is a
 *  float type.
 */
std::tuple<GLenum, bool> to_opengl_format(iris::VertexAttributeType type)
{
    GLenum format = 0u;
    auto is_float = true;

    switch (type)
    {
        case iris::VertexAttributeType::FLOAT_3:
        case iris::VertexAttributeType::FLOAT_4: format = GL_FLOAT; break;
        case iris::VertexAttributeType::UINT32_1:
        case iris::VertexAttributeType::UINT32_4:
            format = GL_UNSIGNED_INT;
            is_float = false;
            break;
        default: throw iris::Exception("unknown vertex attribute type");
    }

    return {format, is_float};
}

}

namespace iris
{

OpenGLMesh::OpenGLMesh(
    const std::vector<VertexData> &vertices,
    const std::vector<std::uint32_t> &indices,
    const VertexAttributes &attributes)
    : Mesh(vertices, indices)
    , vertex_buffer_(vertices_)
    , index_buffer_(indices_)
    , vao_(0u)
{
    // create vao
    ::glGenVertexArrays(1, &vao_);
    expect(check_opengl_error, "could not generate vao");

    bind();

    // ensure both buffers are bound for the vao
    ::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_.handle());
    expect(check_opengl_error, "could not bind buffer");
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_.handle());
    expect(check_opengl_error, "could not bind buffer");

    auto index = 0u;

    // define the vertex attribute data for opengl
    for (const auto &attribute : attributes)
    {
        const auto &[type, components, _, offset] = attribute;

        ::glEnableVertexAttribArray(index);
        expect(check_opengl_error, "could not enable attribute");

        const auto &[open_gl_type, is_float] = to_opengl_format(type);

        if (is_float)
        {
            ::glVertexAttribPointer(
                index,
                static_cast<GLint>(components),
                open_gl_type,
                GL_FALSE,
                static_cast<GLsizei>(attributes.size()),
                reinterpret_cast<void *>(offset));
            expect(check_opengl_error, "could not set attributes");
        }
        else
        {
            ::glVertexAttribIPointer(
                index,
                static_cast<GLint>(components),
                open_gl_type,
                static_cast<GLsizei>(attributes.size()),
                reinterpret_cast<void *>(offset));
            expect(check_opengl_error, "could not set attributes");
        }

        ++index;
    }

    unbind();
}

OpenGLMesh::~OpenGLMesh()
{
    ::glDeleteVertexArrays(1u, &vao_);
}

void OpenGLMesh::update_vertex_data(const std::vector<VertexData> &data)
{
    vertex_buffer_.write(data);
}

void OpenGLMesh::update_index_data(const std::vector<std::uint32_t> &data)
{
    index_buffer_.write(data);
}

GLsizei OpenGLMesh::element_count() const
{
    return static_cast<GLsizei>(index_buffer_.element_count());
}

void OpenGLMesh::bind() const
{
    ::glBindVertexArray(vao_);
    expect(check_opengl_error, "could not bind vao");
}

void OpenGLMesh::unbind() const
{
    ::glBindVertexArray(0u);
    expect(check_opengl_error, "could not unbind vao");
}

}
