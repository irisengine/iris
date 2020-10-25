#include "graphics/mesh_factory.h"

#include <cstdint>
#include <queue>
#include <stack>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "core/exception.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/bone.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/mesh.h"
#include "graphics/mesh_loader.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"
#include "log/log.h"
#include "platform/resource_loader.h"

namespace iris::mesh_factory
{

std::vector<Mesh> sprite(const Vector3 &colour, Texture *texture)
{
    std::vector<Mesh> meshes;

    std::vector<vertex_data> verticies{
        {{-0.5f, 0.5f, 0.0f}, {}, colour, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {}, colour, {1.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {}, colour, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {}, colour, {0.0f, 0.0f, 0.0f}}};

    std::vector<std::uint32_t> indices{0, 2, 1, 3, 2, 0};

    BufferDescriptor descriptor(
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes);

    meshes.emplace_back(std::move(descriptor), texture);

    return meshes;
}

std::vector<Mesh> cube(const Vector3 colour)
{
    std::vector<Mesh> meshes;

    std::vector<vertex_data> verticies{
        {{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, colour, {}},
        {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, colour, {}},
        {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, colour, {}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, colour, {}},
        {{0.999999f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, colour, {}},
        {{1.0f, 1.0f, -0.999999f}, {0.0f, 1.0f, 0.0f}, colour, {}},
        {{1.0f, 1.0f, -0.999999f}, {1.0f, -0.0f, 0.0f}, colour, {}},
        {{1.0f, -1.0f, 1.0f}, {1.0f, -0.0f, 0.0f}, colour, {}},
        {{1.0f, -1.0f, -1.0f}, {1.0f, -0.0f, 0.0f}, colour, {}},
        {{0.999999f, 1.0f, 1.0f}, {0.0f, -0.0f, 1.0f}, colour, {}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f, -0.0f, 1.0f}, colour, {}},
        {{1.0f, -1.0f, 1.0f}, {0.0f, -0.0f, 1.0f}, colour, {}},
        {{-1.0f, -1.0f, 1.0f}, {-1.0f, -0.0f, -0.0f}, colour, {}},
        {{-1.0f, 1.0f, -1.0f}, {-1.0f, -0.0f, -0.0f}, colour, {}},
        {{-1.0f, -1.0f, -1.0f}, {-1.0f, -0.0f, -0.0f}, colour, {}},
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, colour, {}},
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, colour, {}},
        {{1.0f, 1.0f, -0.999999f}, {0.0f, 0.0f, -1.0f}, colour, {}},
        {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, colour, {}},
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, colour, {}},
        {{0.999999f, 1.0f, 1.0f}, {1.0f, -0.0f, 0.0f}, colour, {}},
        {{-1.0f, 1.0f, 1.0f}, {0.0f, -0.0f, 1.0f}, colour, {}},
        {{-1.0f, 1.0f, 1.0f}, {-1.0f, -0.0f, -0.0f}, colour, {}},
        {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, colour, {}}};

    std::vector<std::uint32_t> indices{0, 1,  2,  3,  4,  5,  6,  7,  8,
                                       9, 10, 11, 12, 13, 14, 15, 16, 17,
                                       0, 18, 1,  3,  19, 4,  6,  20, 7,
                                       9, 21, 10, 12, 22, 13, 15, 23, 16};

    BufferDescriptor descriptor(
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes);

    meshes.emplace_back(std::move(descriptor));

    return meshes;
}

std::vector<Mesh> quad(
    const Vector3 &colour,
    const Vector3 &lower_left,
    const Vector3 &lower_right,
    const Vector3 &upper_left,
    const Vector3 &upper_right)
{
    std::vector<Mesh> meshes;

    std::vector<vertex_data> verticies{
        {upper_left, {}, colour, {0.0f, 1.0f, 0.0f}},
        {upper_right, {}, colour, {1.0f, 1.0f, 0.0f}},
        {lower_right, {}, colour, {1.0f, 0.0f, 0.0f}},
        {lower_left, {}, colour, {0.0f, 0.0f, 0.0f}}};

    std::vector<std::uint32_t> indices{0, 2, 1, 3, 2, 0};

    BufferDescriptor descriptor(
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes);

    meshes.emplace_back(std::move(descriptor));

    return meshes;
}

std::tuple<std::vector<Mesh>, Skeleton> load(const std::string &mesh_file)
{
    struct Cache
    {
        std::vector<std::vector<vertex_data>> vertices;
        std::vector<std::vector<std::uint32_t>> indices;
        std::vector<Texture *> textures;
        Skeleton skeleton;
    };

    static std::map<std::string, Cache> cache{};

    std::vector<Mesh> meshes;
    Skeleton *skeleton = nullptr;

    if (cache.count(mesh_file) == 0)
    {
        Cache c;

        load_mesh(mesh_file, &c.vertices, &c.indices, &c.textures, &c.skeleton);

        for (auto i = 0u; i < c.vertices.size(); ++i)
        {
            // we can use the default attributes
            meshes.emplace_back(
                BufferDescriptor(
                    Buffer(c.vertices[i], BufferType::VERTEX_ATTRIBUTES),
                    Buffer(c.indices[i], BufferType::VERTEX_INDICES),
                    vertex_attributes),
                c.textures[i]);
        }

        cache[mesh_file] = c;
        skeleton = &cache[mesh_file].skeleton;
    }
    else
    {
        auto c = cache[mesh_file];

        for (auto i = 0u; i < c.vertices.size(); ++i)
        {
            // we can use the default attributes
            meshes.emplace_back(
                BufferDescriptor(
                    Buffer(c.vertices[i], BufferType::VERTEX_ATTRIBUTES),
                    Buffer(c.indices[i], BufferType::VERTEX_INDICES),
                    vertex_attributes),
                c.textures[i]);
        }
        skeleton = &cache[mesh_file].skeleton;
    }

    return {std::move(meshes), *skeleton};
}

}
