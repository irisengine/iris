////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "core/colour.h"
#include "core/resource_manager.h"
#include "core/vector3.h"
#include "graphics/animation/animation.h"
#include "graphics/mesh.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"

namespace iris
{

class Texture;

/**
 * Abstract class for creating and managing Mesh objects. This class handles caching and lifetime management of all
 * created objects. Implementers just need to provide a graphics API specific method for creating Mesh objects.
 */
class MeshManager
{
  public:
    /**
     * Struct encapsulating all the data returned when loading a mesh file.
     */
    struct Meshes
    {
        /**
         * Data for a single mesh.
         */
        struct MeshData
        {
            /** Pointer to mesh name. */
            Mesh *mesh;

            /** Name of diffuse colour texture (if found in mesh file). */
            std::string texture_name;
        };

        /** Collection of loaded meshes. */
        std::vector<MeshData> mesh_data;

        /** Collection of animations. */
        std::vector<Animation> animations;

        /** Skeleton for all meshes. */
        Skeleton *skeleton;
    };

    /**
     * Construct a new MeshManager.
     *
     * @param resource_manager
     *   Resource manager object.
     *
     * @param flip_uvs_on_load
     *   True if uvs should be flipped for all loaded meshes, false otherwise.
     */
    MeshManager(ResourceManager &resource_manager, bool flip_uvs_on_load);
    virtual ~MeshManager() = default;

    /**
     * Create a Sprite mesh.
     *
     * @param colour
     *   Colour of sprite.
     *
     * @returns
     *   Mesh for sprite.
     */
    const Mesh *sprite(const Colour &colour);

    /**
     * Create a cube mesh.
     *
     * @param colour
     *   Colour of cube.
     *
     * @returns
     *   Mesh for cube.
     */
    const Mesh *cube(const Colour &colour);

    /**
     * Create a new cube, which is not cached. This should be used when the data of the cube Mesh needs to be mutated.
     * It returns a unique_ptr and therefore is not managed by this Manager.
     *
     * Most of the time you will want to use the cube method instead.
     *
     * @param colour
     *   Colour of cube.
     *
     * @returns
     *   Newly created cube mesh.
     */
    std::unique_ptr<Mesh> unique_cube(const Colour &colour) const;

    /**
     * Create a new Mesh, which is not cached. This should be used when the data of the Mesh needs to be mutated.
     * It returns a unique_ptr and therefore is not managed by this Manager.
     *
     * Most of the time you will want to use the other factory methods instead.
     *
     * @param verticies
     *   Vertex data of mesh.
     *
     * @param indices
     *   Indices of mesh.
     *
     * @returns
     *   Newly created mesh.
     */
    std::unique_ptr<Mesh> unique_mesh(
        const std::vector<iris::VertexData> &vertices,
        const std::vector<std::uint32_t> &indices) const;

    /**
     * Create a (XZ) plane mesh (normals pointing up along Y).
     *
     * @param colour
     *   Colour of plane.
     *
     * @param divisions
     *   Number of divisions (both horizontal and vertical).
     *
     * @param scale
     *   How much to scale each division by.
     *
     * @returns
     *   Mesh for place.
     */
    const Mesh *plane(const Colour &colour, std::uint32_t divisions, float scale = 1.0f);

    /**
     * Create a (XY) height map mesh.
     *
     * @param colour
     *   Colour of height map.
     *
     * @param height_image.
     *   Image data to source heights from (will use r component).
     *
     * @returns
     *   Mesh for heightmap.
     */
    const Mesh *heightmap(const Colour &colour, const Texture *height_image);

    /**
     * Create a Quad mesh.
     *
     * @param colour
     *   Colour of quad.
     *
     * @param lower_left
     *   World coords of lower left of quad.
     *
     * @param lower_right
     *   World coords of lower right of quad.
     *
     * @param upper_left
     *   World coords of upper left of quad.
     *
     * @param upper_right
     *   World coords of upper right of quad.
     *
     * @returns
     *   Mesh for quad.
     */
    const Mesh *quad(
        const Colour &colour,
        const Vector3 &lower_left,
        const Vector3 &lower_right,
        const Vector3 &upper_left,
        const Vector3 &upper_right);

    /**
     * Load a mesh from file.
     *
     * @param mesh_file
     *   File to load.
     *
     * @returns
     *   Mesh loaded from file.
     */
    Meshes load_mesh(const std::string &mesh_file);

    /**
     * Load a skeleton from a file.
     *
     * Note that unlike load_mesh this returns a new copy each time, this is so
     * each Skeleton can be mutated independently.
     *
     * @param mesh_file
     *   File to load.
     *
     * @returns
     *   Skeleton loaded from file.
     */
    // Skeleton* load_skeleton(const std::string &mesh_file);

    /**
     * Load animations from a file.
     *
     * Note that unlike load_mesh this returns a new copy each time, this is so
     * each Skeleton can be mutated independently.
     *
     * @param mesh_file
     *   File to load.
     *
     * @returns
     *   Animations loaded from file.
     */
    // std::vector<Animation> load_animations(const std::string &mesh_file);

  protected:
    /**
     * Create a Mesh object from the provided vertex and index data.
     *
     * @param vertices
     *   Collection of vertices for the Mesh.
     *
     * @param indices
     *   Collection of indices for the Mesh.
     *
     * @returns
     *   Loaded Mesh.
     */
    virtual std::unique_ptr<Mesh> create_mesh(
        const std::vector<VertexData> &vertices,
        const std::vector<std::uint32_t> &indices) const = 0;

  private:
    /**
     * Internal struct for caching loaded mesh data.
     */
    struct LoadedMesh
    {
        std::unique_ptr<Mesh> mesh;
        std::string texture_name;
    };

    /** Resource manager object. */
    ResourceManager &resource_manager_;

    /** Cache of created Mesh objects. */
    std::unordered_map<std::string, std::vector<LoadedMesh>> loaded_meshes_;

    /** Collection of created Animation objects. */
    std::unordered_map<std::string, std::vector<Animation>> loaded_animations_;

    /** Collection of loaded skeletons - these are the "master" copies. */
    std::unordered_map<std::string, Skeleton> loaded_skeletons_;

    /** Collection of skeletons returned to callers. */
    std::deque<Skeleton> skeleton_copies_;

    /** Flag indicating if uvs should be flipped for loaded meshes. */
    bool flip_uvs_on_load_;
};

}
