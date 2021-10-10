////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

#import <Metal/Metal.h>

#include "graphics/mesh.h"
#include "graphics/metal/metal_buffer.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Implementation of Mesh for metal.
 */
class MetalMesh : public Mesh
{
  public:
    /**
     * Construct a new MetalMesh.
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
    MetalMesh(
        const std::vector<VertexData> &vertices,
        const std::vector<std::uint32_t> &indices,
        const VertexAttributes &attributes);

    ~MetalMesh() override = default;

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
     * Get vertex buffer.
     *
     * @returns
     *   Const reference to vertex buffer object.
     */
    const MetalBuffer &vertex_buffer() const;

    /**
     * Get index buffer.
     *
     * @returns
     *   Const reference to index buffer object.
     */
    const MetalBuffer &index_buffer() const;

    /**
     * Get Metal object which describes vertex layout.
     *
     * @param
     *   Metal object describing vertex.
     */
    MTLVertexDescriptor *descriptors() const;

  private:
    /** Buffer for vertex data. */
    MetalBuffer vertex_buffer_;

    /** Buffer for index data. */
    MetalBuffer index_buffer_;

    /** Metal object describing vertex layout. */
    MTLVertexDescriptor *descriptors_;
};

}
