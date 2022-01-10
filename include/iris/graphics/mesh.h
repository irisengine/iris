////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Abstract class for a Mesh - a class which encapsulates all the vertex data needed to render a mesh.
 */
class Mesh
{
  public:
    /**
     * Construct a new Mesh.
     *
     * @param vertices
     *   Collection of vertices for mesh.
     *
     * @param indices
     *   Collection of indices for mesh.
     */
    Mesh(const std::vector<VertexData> &vertices, const std::vector<std::uint32_t> &indices);

    virtual ~Mesh() = default;

    /**
     * Update the vertex data, this will also update any GPU data.
     *
     * @param data
     *   New vertex data.
     */
    virtual void update_vertex_data(const std::vector<VertexData> &data) = 0;

    /**
     * Update the index data, this will also update any GPU data.
     *
     * @param data
     *   New index data.
     */
    virtual void update_index_data(const std::vector<std::uint32_t> &data) = 0;

    /**
     * Get a const reference to the vertex data.
     *
     * @returns
     *   Constant reference to vertex data.
     */
    const std::vector<VertexData> &vertices() const;

    /**
     * Get a const reference to the index data.
     *
     * @returns
     *   Constant reference to index data.
     */
    const std::vector<std::uint32_t> &indices() const;

  protected:
    /** Vertex data. */
    std::vector<VertexData> vertices_;

    /** Index data. */
    std::vector<std::uint32_t> indices_;
};

}
