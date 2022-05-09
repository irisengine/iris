////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_mesh_manager.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/opengl/opengl_mesh.h"
#include "graphics/vertex_data.h"

namespace iris
{

OpenGLMeshManager::OpenGLMeshManager()
    : MeshManager(true)
{
}

std::unique_ptr<Mesh> OpenGLMeshManager::create_mesh(
    const std::vector<iris::VertexData> &vertices,
    const std::vector<std::uint32_t> &indices) const
{
    return std::make_unique<OpenGLMesh>(vertices, indices, DefaultVertexAttributes);
}

}
