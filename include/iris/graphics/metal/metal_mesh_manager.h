////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "core/resource_manager.h"
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
    /**
     * Construct a new MetalMeshManager.
     *
     * @param resource_manager
     *   Resource manager object.
     */
    MetalMeshManager(ResourceManager &resource_manager);

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
