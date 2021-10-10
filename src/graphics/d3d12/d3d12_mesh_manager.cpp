////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_mesh_manager.h"

#include <cstdint>
#include <memory>

#include "graphics/d3d12/d3d12_mesh.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/vertex_data.h"

namespace iris
{

std::unique_ptr<Mesh> D3D12MeshManager::create_mesh(
    const std::vector<iris::VertexData> &vertices,
    const std::vector<std::uint32_t> &indices) const
{
    return std::make_unique<D3D12Mesh>(
        vertices, indices, DefaultVertexAttributes);
}

}
