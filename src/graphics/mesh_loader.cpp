////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/mesh_loader.h"

#include <cstdint>
#include <functional>
#include <stack>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "core/colour.h"
#include "core/error_handling.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/resource_loader.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/animation/animation.h"
#include "graphics/bone.h"
#include "graphics/skeleton.h"
#include "graphics/vertex_data.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to convert an assimp Matrix to an engine one.
 *
 * @param matrix
 *   Assimp matrix.
 *
 * @returns
 *   Engine matrix.
 */
iris::Matrix4 convert_matrix(const ::aiMatrix4x4 &matrix)
{
    return iris::Matrix4{
        {{matrix.a1,
          matrix.a2,
          matrix.a3,
          matrix.a4,
          matrix.b1,
          matrix.b2,
          matrix.b3,
          matrix.b4,
          matrix.c1,
          matrix.c2,
          matrix.c3,
          matrix.c4,
          matrix.d1,
          matrix.d2,
          matrix.d3,
          matrix.d4}}};
}

/**
 * Get the animations from an assimp scene.
 *
 * @param scene
 *   Scene to get animations from.
 *
 * @returns
 *   Animations.
 */
std::vector<iris::Animation> process_animations(const ::aiScene *scene)
{
    std::vector<iris::Animation> animations{};

    // parse each animation
    for (auto i = 0u; i < scene->mNumAnimations; ++i)
    {
        const auto *animation = scene->mAnimations[i];

        auto ticks_per_second = animation->mTicksPerSecond;
        if (ticks_per_second == 0.0f)
        {
            LOG_WARN("ms", "no ticks per second - guessing");
            ticks_per_second = 33.0f;
        }

        const std::chrono::milliseconds tick_time{static_cast<std::uint32_t>((1.0f / ticks_per_second) * 1000u)};
        const auto duration = tick_time * static_cast<std::uint32_t>(animation->mDuration);

        std::map<std::string, std::vector<iris::KeyFrame>, std::less<>> nodes;

        // each channel is a collection of keys
        for (auto j = 0u; j < animation->mNumChannels; ++j)
        {
            const auto *channel = animation->mChannels[j];

            // sanity check we have an equal number of keys for position,
            // rotation and scale
            iris::ensure(
                (channel->mNumPositionKeys == channel->mNumRotationKeys) ||
                    (channel->mNumRotationKeys == channel->mNumScalingKeys),
                "incomplete frame data");

            std::vector<iris::KeyFrame> keyframes{};

            // convert assimp keys into keyframes
            for (auto k = 0u; k < channel->mNumPositionKeys; ++k)
            {
                const auto assimp_pos = channel->mPositionKeys[k].mValue;
                const auto assimp_rot = channel->mRotationKeys[k].mValue;
                const auto assimp_scale = channel->mScalingKeys[k].mValue;
                const auto time = static_cast<std::uint32_t>(channel->mPositionKeys[k].mTime);

                keyframes.emplace_back(
                    iris::Transform{
                        {assimp_pos.x, assimp_pos.y, assimp_pos.z},
                        {assimp_rot.x, assimp_rot.y, assimp_rot.z, assimp_rot.w},
                        {assimp_scale.x, assimp_scale.y, assimp_scale.z}},
                    time * tick_time);
            }

            nodes[channel->mNodeName.C_Str()] = keyframes;
        }

        animations.emplace_back(duration, animation->mName.C_Str(), nodes);
    }

    return animations;
}

/**
 * Get the bones from an assimp scene.
 *
 * Assimp has two separate concepts for animation data:
 *   bone - offset matrix and collection of vertices it effects
 *   node - hierarchical object containing transformation
 *
 * An assimp node may refer to a bone, or it may just represent an intermediate
 * transformation between bones. We unify both of these assimp concepts into an
 * engine Bone. Our Bone may or may not effect vertices and maintains the same
 * hierarchy as the assimp nodes.
 *
 * @param scene
 *   Scene to get bones from.
 *
 * @returns
 *   Bones.
 */
std::vector<iris::Bone> process_bones(const aiScene *scene)
{
    std::vector<iris::Bone> bones{};

    std::stack<std::tuple<const ::aiNode *, std::string>> nodes;
    nodes.emplace(scene->mRootNode, std::string{});

    // walk the node hierarchy
    do
    {
        auto [node, parent_name] = nodes.top();
        nodes.pop();

        const std::string name{node->mName.C_Str()};

        // create a bone which represents the nodes transformation but effects
        // no vertices
        iris::Bone bone{name, parent_name, {}, convert_matrix(node->mTransformation)};

        for (auto i = 0u; i < scene->mNumMeshes; ++i)
        {
            const auto *mesh = scene->mMeshes[i];

            // see if this node represents an assimp bone
            for (auto j = 0u; j < mesh->mNumBones; ++j)
            {
                const ::aiBone *ai_bone = mesh->mBones[j];

                if (std::string(ai_bone->mName.C_Str()) == name)
                {
                    // replace bone with one that stores the weights as well the correct matrices
                    bone = {
                        name,
                        parent_name,
                        convert_matrix(ai_bone->mOffsetMatrix),
                        convert_matrix(node->mTransformation)};

                    break;
                }
            }
        }

        bones.emplace_back(bone);

        for (auto i = 0u; i < node->mNumChildren; ++i)
        {
            nodes.emplace(node->mChildren[i], name);
        }
    } while (!nodes.empty());

    // always return at least one default bone
    if (bones.empty())
    {
        bones.emplace_back("root", "", iris::Matrix4{}, iris::Matrix4{});
    }

    return bones;
}

/**
 * Get the weights for a given assimp mesh.
 *
 * @param mesh
 *   Mesh to get weights for.
 *
 * @returns
 *   Collection of bone weights.
 */
std::vector<iris::Weight> process_weights(const aiMesh *mesh)
{
    std::vector<iris::Weight> weights{};

    for (auto j = 0u; j < mesh->mNumBones; ++j)
    {
        const ::aiBone *ai_bone = mesh->mBones[j];

        for (auto k = 0u; k < ai_bone->mNumWeights; ++k)
        {
            const auto &weight = ai_bone->mWeights[k];
            weights.push_back(
                {.vertex = weight.mVertexId, .weight = weight.mWeight, .bone_name = ai_bone->mName.C_Str()});
        }
    }

    return weights;
}

/**
 * Get the indices for the given mesh.
 *
 * @param mesh
 *   Mesh to get vertices for.
 *
 * @returns
 *   Indices.
 */
std::vector<std::uint32_t> process_indices(const ::aiMesh *mesh)
{
    std::vector<std::uint32_t> indices{};

    for (auto i = 0u; i < mesh->mNumFaces; ++i)
    {
        const auto &face = mesh->mFaces[i];
        for (auto j = 0u; j < face.mNumIndices; ++j)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return indices;
}

/**
 * Get the vertices for the given mesh.
 *
 * @param mesh
 *   Mesh to get vertices for.
 *
 * @param material
 *   Material for current mesh.
 *
 * @returns
 *   Vertex data.
 */
std::vector<iris::VertexData> process_vertices(const ::aiMesh *mesh, const ::aiMaterial *material)
{
    std::vector<iris::VertexData> vertices{};

    for (auto i = 0u; i < mesh->mNumVertices; ++i)
    {
        const auto &vertex = mesh->mVertices[i];
        const auto &normal = mesh->mNormals[i];
        iris::Colour colour{1.0f, 1.0f, 1.0f};
        iris::Vector3 texture_coords{};
        iris::Vector3 tangent{};
        iris::Vector3 bitangent{};

        // get texture coordinates if they exist
        if (mesh->HasTextureCoords(0))
        {
            texture_coords.x = mesh->mTextureCoords[0][i].x;
            texture_coords.y = mesh->mTextureCoords[0][i].y;

            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;

            bitangent.x = mesh->mBitangents[i].x;
            bitangent.y = mesh->mBitangents[i].y;
            bitangent.z = mesh->mBitangents[i].z;
        }

        // only support diffuse colour
        ::aiColor3D c(0.f, 0.f, 0.f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, c);
        colour.r = c.r;
        colour.g = c.g;
        colour.b = c.b;

        vertices.emplace_back(
            iris::Vector3(vertex.x, vertex.y, vertex.z),
            iris::Vector3(normal.x, normal.y, normal.z),
            colour,
            texture_coords,
            tangent,
            bitangent);
    }

    return vertices;
}
}

namespace iris::mesh_loader
{

void load(
    const std::string &mesh_name,
    bool flip_uvs,
    MeshDataCallback mesh_data_callback,
    AnimationCallback animation_callback)
{
    const auto file_data = ResourceLoader::instance().load(mesh_name);

    const auto import_flags = flip_uvs ? aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs
                                       : aiProcess_Triangulate | aiProcess_CalcTangentSpace;

    // parse file using assimp
    ::Assimp::Importer importer{};
    const auto *scene = importer.ReadFileFromMemory(file_data.data(), file_data.size(), import_flags);

    ensure(
        (scene != nullptr) && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && (scene->mRootNode != nullptr),
        std::string{"could not load mesh: "} + importer.GetErrorString());

    if (scene->mAnimations != 0u)
    {
        animation_callback(process_animations(scene), {process_bones(scene)});
    }

    const auto *root = scene->mRootNode;

    std::stack<const aiNode *> to_process;
    to_process.emplace(root);

    // walk the assimp scene
    do
    {
        const auto *node = to_process.top();
        to_process.pop();

        for (auto i = 0u; i < node->mNumMeshes; ++i)
        {
            const auto *mesh = scene->mMeshes[node->mMeshes[i]];
            const auto *material = scene->mMaterials[mesh->mMaterialIndex];

            std::string texture_name;
            aiString assimp_str{};
            if (material->GetTexture(aiTextureType_DIFFUSE, 0u, &assimp_str) == aiReturn_SUCCESS)
            {
                texture_name = assimp_str.C_Str();
            }

            mesh_data_callback(
                process_vertices(mesh, material), process_indices(mesh), process_weights(mesh), texture_name);
        }

        // add child nodes so we visit all meshes
        for (auto i = 0u; i < node->mNumChildren; ++i)
        {
            to_process.emplace(node->mChildren[i]);
        }
    } while (!to_process.empty());
}

}
