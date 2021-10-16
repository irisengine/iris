////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "graphics/d3d12/d3d12_buffer.h"
#include "graphics/mesh.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Implementation of d3d12 for metal.
 */
class D3D12Mesh : public Mesh
{
  public:
    /**
     * Construct a new D3D12Mesh.
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
    D3D12Mesh(
        const std::vector<VertexData> &vertices,
        const std::vector<std::uint32_t> &indices,
        const VertexAttributes &attributes);

    ~D3D12Mesh() override = default;

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
    const D3D12Buffer &vertex_buffer() const;

    /**
     * Get index buffer.
     *
     * @returns
     *   Const reference to index buffer object.
     */
    const D3D12Buffer &index_buffer() const;

    /**
     * Get d3d12 object which describes vertex layout.
     *
     * @param
     *   D3D12 object describing vertex.
     */
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_descriptors() const;

  private:
    /** Buffer for vertex data. */
    D3D12Buffer vertex_buffer_;

    /** Buffer for index data. */
    D3D12Buffer index_buffer_;

    /** D3D12 object describing vertex layout. */
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_descriptors_;
};

}
