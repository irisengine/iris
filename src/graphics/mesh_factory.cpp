#include "graphics/mesh_factory.h"

#include <cstdint>
#include <queue>
#include <stack>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "core/exception.h"
#include "core/vector3.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"
#include "log/log.h"
#include "platform/resource_loader.h"

namespace iris::mesh_factory
{

std::vector<Mesh> sprite(const Vector3 &colour, Texture &&texture)
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

    meshes.emplace_back(std::move(descriptor), std::move(texture));

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

std::vector<Mesh> load(const std::string &mesh_file)
{
    std::vector<Mesh> meshes;

    const auto file_data = ResourceLoader::instance().load(mesh_file);

    ::Assimp::Importer importer{};
    const auto *scene = importer.ReadFile(mesh_file, ::aiProcess_Triangulate);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        scene->mRootNode == nullptr)
    {
        throw Exception(
            std::string{"could not load mesh: "} + importer.GetErrorString());
    }

    std::stack<::aiNode *> to_process;
    to_process.emplace(scene->mRootNode);

    do
    {
        const auto *node = to_process.top();
        to_process.pop();

        for (auto i = 0u; i < node->mNumMeshes; ++i)
        {
            std::vector<vertex_data> vertices;
            std::vector<std::uint32_t> indices;
            const auto *mesh = scene->mMeshes[node->mMeshes[i]];
            const auto *material = scene->mMaterials[mesh->mMaterialIndex];
            auto texture = Texture::blank();

            aiString name;
            material->Get(AI_MATKEY_NAME, name);

            for (auto j = 0u; j < mesh->mNumVertices; ++j)
            {
                const auto &vertex = mesh->mVertices[j];
                const auto &normal = mesh->mNormals[j];
                Vector3 colour{1.0f, 1.0f, 1.0f};
                Vector3 texture_coords{};

                if (mesh->HasTextureCoords(0))
                {
                    texture_coords.x = mesh->mTextureCoords[0][j].x;
                    texture_coords.y = mesh->mTextureCoords[0][j].y;
                }

                aiColor3D c(0.f, 0.f, 0.f);
                material->Get(AI_MATKEY_COLOR_DIFFUSE, c);
                colour.x = c.r;
                colour.y = c.g;
                colour.z = c.b;

                vertices.emplace_back(
                    Vector3(vertex.x, vertex.y, vertex.z),
                    Vector3(normal.x, normal.y, normal.z),
                    colour,
                    texture_coords);
            }

            for (auto j = 0u; j < mesh->mNumFaces; ++j)
            {
                const auto &face = mesh->mFaces[j];
                for (auto k = 0u; k < face.mNumIndices; ++k)
                {
                    indices.emplace_back(face.mIndices[k]);
                }
            }

            const auto type = ::aiTextureType_DIFFUSE;

            for (auto j = 0u; j < material->GetTextureCount(type); j++)
            {
                ::aiString str;
                material->GetTexture(type, i, &str);

                const auto texture_path = "assets/" + std::string{str.C_Str()};

                texture = Texture{texture_path};
            }

            for (auto j = 0u; j < mesh->mNumBones; ++j)
            {
                const auto *bone = mesh->mBones[j];
                LOG_INFO(
                    "mesh",
                    "bone | name: {} weights: {}",
                    bone->mName.C_Str(),
                    bone->mNumWeights);
            }

            BufferDescriptor descriptor(
                Buffer(vertices, BufferType::VERTEX_ATTRIBUTES),
                Buffer(indices, BufferType::VERTEX_INDICES),
                vertex_attributes);

            meshes.emplace_back(std::move(descriptor), std::move(texture));
        }

        for (auto i = 0u; i < node->mNumChildren; ++i)
        {
            to_process.emplace(node->mChildren[i]);
            LOG_INFO("mesh", "adding child");
        }
    } while (!to_process.empty());

    return meshes;
}
}
