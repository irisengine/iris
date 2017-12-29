#include "entity.hpp"

#include <cstdint>
#include <experimental/filesystem>
#include <queue>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "matrix.hpp"
#include "vector3.hpp"

namespace
{

/**
 * Helper function to convert an assimp aiMesh object into an eng::mesh object.
 *
 * @param mesh
 *   Assimp mesh object.
 *
 * @param path
 *   Path to model file to load.
 *
 * @param colour
 *   Colour of meshes.
 *
 * @param position
 *   Position of mesh in world space.
 *
 * @param scale
 *   Scale of entity.
 *
 * @returns
 *   An eng::mesh object representing the supplied asismp aiMesh.
 */
eng::mesh create_mesh(
    const aiMesh * const mesh,
    const std::uint32_t colour,
    const eng::vector3 &position,
    const eng::vector3 &scale)
{
    if(mesh == nullptr)
    {
        throw std::runtime_error("mesh pointer is null");
    }

    std::vector<eng::vertex_data> vertices{ };
    std::vector<std::uint32_t> indices{ };

    for(auto j = 0u; j < mesh->mNumVertices; ++j)
    {
        // parse each assimp vertex data
        const auto ai_vertex = mesh->mVertices[j];
        vertices.emplace_back(eng::vector3{
            ai_vertex.x,
            ai_vertex.y,
            ai_vertex.z });
    }

    // parse each assimp index
    for(auto j = 0u; j < mesh->mNumFaces; ++j)
    {
        const auto face = mesh->mFaces[j];

        for(auto k = 0u; k < face.mNumIndices; ++k)
        {
            indices.emplace_back(face.mIndices[k]);
        }
    }

    return{ vertices, indices, colour, position, scale };
}

/**
 * Helper function to parse a 3d model file and create an internal engine
 * representation from it.
 *
 * Internally this method uses the assimp library so will load any format
 * it supports.
 * @param path
 *   Path to model file to load.
 *
 * @param colour
 *   Colour of meshes.
 *
 * @param position
 *   Position of mesh in world space.
 *
 * @param scale
 *   Scale of entity.
 *
 * @returns
 *   Collection of parsed meshes.
 */
std::vector<eng::mesh> load_file(
    const std::experimental::filesystem::path &path,
    const std::uint32_t colour,
    const eng::vector3 &position,
    const eng::vector3 &scale)
{
    if(!std::experimental::filesystem::exists(path))
    {
        throw std::runtime_error(path.string() + " does not exist");
    }

    ::Assimp::Importer importer{ };

    // parse the mesh using assimp and check it was successful
    const auto *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    if((scene == nullptr) ||
       (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) ||
       (scene->mRootNode == nullptr))
    {
        throw std::runtime_error("could not load file: " + std::string{ importer.GetErrorString() });
    }

    std::vector<eng::mesh> meshes{ };

    std::queue<const aiNode * const> queue;
    queue.push(scene->mRootNode);

    // iterate through the assimp mesh hierarchy and convert each mesh into
    // our own representation
    do
    {
        const auto *node = queue.front();
        queue.pop();

        // convert each mesh in the current node
        for(int i = 0u; i < node->mNumMeshes; ++i)
        {
            const auto *mesh = scene->mMeshes[node->mMeshes[i]];

            meshes.emplace_back(create_mesh(
                mesh,
                colour,
                position,
                scale));
        }

        // queue up the child nodes for processing
        for(int i = 0u; i < node->mNumChildren; ++i)
        {
            queue.push(node->mChildren[i]);
        }
    } while(!queue.empty());

    return meshes;
}

}
namespace eng
{

entity::entity(
    const std::experimental::filesystem::path &path,
    const std::uint32_t colour,
    const vector3 &position,
    const vector3 &scale)
    : meshes_(load_file(path, colour, position, scale))
{ }

const std::vector<mesh>& entity::meshes() const noexcept
{
    return meshes_;
}

}

