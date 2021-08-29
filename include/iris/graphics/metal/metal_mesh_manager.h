#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Implementation of MeshManager for metal.
 */
class MetalMeshManager : public MeshManager
{
  public:
    ~MetalMeshManager() override = default;

  protected:
    /**
     * Create a Mesh object from the provided vertex and index data.
     *
     * @param vertices
     *   Collection of vertices for the Mesh.
     *
     * @param indices
     *   Collection of indices fro the Mesh.
     *
     * @returns
     *   Loaded Mesh.
     */
    std::unique_ptr<Mesh> create_mesh(
        const std::vector<iris::VertexData> &vertices,
        const std::vector<std::uint32_t> &indices) const override;
};

}
