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

    std::vector<vertex_data> vertices{
        {{-0.5f, 0.5f, 0.0f}, {}, colour, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {}, colour, {1.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {}, colour, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {}, colour, {0.0f, 0.0f, 0.0f}}};

    std::vector<std::uint32_t> indices{0, 2, 1, 3, 2, 0};

    meshes.emplace_back(vertices, indices, std::move(texture));

    return meshes;
}

std::vector<Mesh> cube(const Vector3 colour)
{
    std::vector<Mesh> meshes;

    std::vector<vertex_data> vertices{
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

    meshes.emplace_back(vertices, indices, Texture::blank());

    return meshes;
}

std::vector<Mesh> load(const std::string &mesh_file)
{
    const auto file_data = ResourceLoader::instance().load(mesh_file);

    ::Assimp::Importer importer{};
    const auto *scene = importer.ReadFile(
        mesh_file, ::aiProcess_Triangulate | ::aiProcess_FlipUVs);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        scene->mRootNode == nullptr)
    {
        throw Exception(
            std::string{"could not load mesh: "} + importer.GetErrorString());
    }

    std::stack<::aiNode *> to_process;
    to_process.emplace(scene->mRootNode);

    std::vector<Mesh> meshes;

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

            meshes.emplace_back(vertices, indices, std::move(texture));
        }

        for (auto i = 0u; i < node->mNumChildren; ++i)
        {
            to_process.emplace(node->mChildren[i]);
            LOG_INFO("mesh", "adding child");
        }
    } while (!to_process.empty());

    return meshes;
    // return {vertices, indices, Texture::blank()};
}
}
