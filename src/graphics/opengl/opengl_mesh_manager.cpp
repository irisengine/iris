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

std::unique_ptr<Mesh> OpenGLMeshManager::create_mesh(
    const std::vector<iris::VertexData> &vertices,
    const std::vector<std::uint32_t> &indices) const
{
    return std::make_unique<OpenGLMesh>(
        vertices, indices, DefaultVertexAttributes);
}

}
