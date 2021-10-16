////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

#include "graphics/mesh.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_buffer.h"
#include "graphics/vertex_attributes.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Implementation of Mesh for OpenGL.
 */
class OpenGLMesh : public Mesh
{
  public:
    /**
     * Construct a new OpenGLMesh.
     *
     * @param vertices
     *   Vertices for the mesh.
     *
     * @param indices
     *   Indices for the mesh.
     *
     * @param attributes
     *   Attributes of the vertices.
     */
    OpenGLMesh(
        const std::vector<VertexData> &vertices,
        const std::vector<std::uint32_t> &indices,
        const VertexAttributes &attributes);

    /**
     * Clean up OpenGL objects.
     */
    ~OpenGLMesh();

    /**
     * Update the vertex data, this will also update any GPU data.
     *
     * @param data
     *   New vertex data.
     */
    void update_vertex_data(const std::vector<VertexData> &data) override;

    /**
     * Update the index data, this will also update any GPU data.
     *
     * @param data
     *   New index data.
     */
    void update_index_data(const std::vector<std::uint32_t> &data) override;

    /**
     * Get number of elements to be rendered.
     *
     * @returns
     *   Number of elements to render.
     */
    GLsizei element_count() const;

    /**
     * Bind this mesh for rendering.
     */
    void bind() const;

    /**
     * Unbind this mesh for rendering.
     */
    void unbind() const;

  private:
    /** Buffer for vertex data. */
    OpenGLBuffer vertex_buffer_;

    /** Buffer for index data. */
    OpenGLBuffer index_buffer_;

    /** OpenGL handle to a vertex array object for this mesh. */
    GLuint vao_;
};

}
