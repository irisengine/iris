////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_mesh_manager.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "core/resource_manager.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/metal/metal_mesh.h"
#include "graphics/vertex_data.h"

namespace iris
{

MetalMeshManager::MetalMeshManager(ResourceManager &resource_manager)
    : MeshManager(resource_manager, false)
{
}

std::unique_ptr<Mesh> MetalMeshManager::create_mesh(
    const std::vector<iris::VertexData> &vertices,
    const std::vector<std::uint32_t> &indices) const
{
    return std::make_unique<MetalMesh>(vertices, indices, DefaultVertexAttributes);
}

}
