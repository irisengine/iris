#include "graphics/mesh_factory.h"

#include <cstdint>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "core/colour.h"
#include "core/exception.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/bone.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/mesh_loader.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"
#include "log/log.h"
#include "platform/resource_loader.h"

namespace iris::mesh_factory
{

BufferDescriptor empty()
{
    std::vector<vertex_data> verticies{};
    std::vector<std::uint32_t> indices{};

    return {
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes};
}

BufferDescriptor sprite(const Colour &colour)
{
    std::vector<vertex_data> verticies{
        {{-1.0, 1.0, 0.0f}, {}, colour, {0.0f, 1.0f, 0.0f}},
        {{1.0, 1.0, 0.0f}, {}, colour, {1.0f, 1.0f, 0.0f}},
        {{1.0, -1.0, 0.0f}, {}, colour, {1.0f, 0.0f, 0.0f}},
        {{-1.0, -1.0, 0.0f}, {}, colour, {0.0f, 0.0f, 0.0f}}};

    std::vector<std::uint32_t> indices{0, 2, 1, 3, 2, 0};

    return {
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes};
}

BufferDescriptor cube(const Colour &colour)
{
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

    return {
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes};
}

BufferDescriptor plane(const Colour &colour, std::uint32_t divisions)
{
    if (divisions == 0)
    {
        throw Exception("divisions must be >= 0");
    }

    std::vector<vertex_data> verticies(
        static_cast<std::size_t>(std::pow(divisions + 1u, 2u)));

    const Vector3 normal{0.0f, 0.0f, 1.0f};
    const Vector3 tangent{1.0f, 0.0f, 0.0f};
    const Vector3 bitangent{0.0f, 1.0f, 0.0f};

    const auto width = 1.0f / static_cast<float>(divisions);

    for (auto y = 0u; y <= divisions; ++y)
    {
        for (auto x = 0u; x <= divisions; ++x)
        {
            verticies[(y * (divisions + 1u)) + x] = {
                {(x * width) - 0.5f, (y * width) - 0.5f, 0.0f},
                normal,
                colour,
                {(x * width), 1.0f - (y * width), 0.0f},
                tangent,
                bitangent};
        }
    }

    std::vector<std::uint32_t> indices{};

    for (auto y = 0u; y < divisions; ++y)
    {
        for (auto x = 0u; x < divisions; ++x)
        {
            indices.emplace_back((y * (divisions + 1u) + x));
            indices.emplace_back(((y + 1) * (divisions + 1u) + x));
            indices.emplace_back((y * (divisions + 1u) + x + 1u));
            indices.emplace_back((y * (divisions + 1u) + x + 1u));
            indices.emplace_back(((y + 1) * (divisions + 1u) + x));
            indices.emplace_back(((y + 1) * (divisions + 1u) + x + 1u));
        }
    }

    return {
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes};
}

BufferDescriptor quad(
    const Colour &colour,
    const Vector3 &lower_left,
    const Vector3 &lower_right,
    const Vector3 &upper_left,
    const Vector3 &upper_right)
{
    std::vector<vertex_data> verticies{
        {upper_left, {}, colour, {0.0f, 1.0f, 0.0f}},
        {upper_right, {}, colour, {1.0f, 1.0f, 0.0f}},
        {lower_right, {}, colour, {1.0f, 0.0f, 0.0f}},
        {lower_left, {}, colour, {0.0f, 0.0f, 0.0f}}};

    std::vector<std::uint32_t> indices{0, 2, 1, 3, 2, 0};

    return {
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indices, BufferType::VERTEX_INDICES),
        vertex_attributes};
}

BufferDescriptor lines(
    const std::vector<Vector3> &line_data,
    const Colour &colour)
{
    std::vector<std::tuple<Vector3, Colour, Vector3, Colour>> data{};

    for (auto i = 0u; i < line_data.size() - 1u; ++i)
    {
        data.emplace_back(line_data[i], colour, line_data[i + 1u], colour);
    }

    return lines(data);
}

BufferDescriptor lines(
    const std::vector<std::tuple<Vector3, Colour, Vector3, Colour>> &line_data)
{
    std::vector<vertex_data> verticies{};
    std::vector<std::uint32_t> indicies;

    for (const auto &[from_position, from_colour, to_position, to_colour] :
         line_data)
    {
        verticies.emplace_back(
            from_position, Vector3{1.0f}, from_colour, Vector3{});
        indicies.emplace_back(
            static_cast<std::uint32_t>(verticies.size() - 1u));

        verticies.emplace_back(
            to_position, Vector3{1.0f}, to_colour, Vector3{});
        indicies.emplace_back(
            static_cast<std::uint32_t>(verticies.size() - 1u));
    }

    return {
        Buffer(verticies, BufferType::VERTEX_ATTRIBUTES),
        Buffer(indicies, BufferType::VERTEX_INDICES),
        vertex_attributes};
}

std::tuple<std::vector<BufferDescriptor>, Skeleton> load(
    const std::string &mesh_file)
{
    struct Cache
    {
        std::vector<std::vector<vertex_data>> vertices;
        std::vector<std::vector<std::uint32_t>> indices;
        std::vector<Texture *> textures;
        Skeleton skeleton;
    };

    static std::map<std::string, Cache> cache{};

    std::vector<BufferDescriptor> meshes;
    Skeleton *skeleton = nullptr;

    if (cache.count(mesh_file) == 0)
    {
        Cache c;

        load_mesh(mesh_file, &c.vertices, &c.indices, &c.textures, &c.skeleton);

        for (auto i = 0u; i < c.vertices.size(); ++i)
        {
            // we can use the default attributes
            meshes.emplace_back(BufferDescriptor(
                Buffer(c.vertices[i], BufferType::VERTEX_ATTRIBUTES),
                Buffer(c.indices[i], BufferType::VERTEX_INDICES),
                vertex_attributes));
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
            meshes.emplace_back(BufferDescriptor(
                Buffer(c.vertices[i], BufferType::VERTEX_ATTRIBUTES),
                Buffer(c.indices[i], BufferType::VERTEX_INDICES),
                vertex_attributes));
        }
        skeleton = &cache[mesh_file].skeleton;
    }

    return {std::move(meshes), *skeleton};
}
}
