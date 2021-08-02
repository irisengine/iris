#pragma once

#include <cstdint>
#include <vector>

#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Interface for a Mesh - a class which encapsulates all the vertex data needed
 * to render a mesh.
 */
class Mesh
{
  public:
    virtual ~Mesh();

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
};

}
