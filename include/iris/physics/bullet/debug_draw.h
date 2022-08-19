////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btVector3.h>

#include "core/exception.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "physics/bullet/bullet_box_collision_shape.h"
#include "physics/bullet/bullet_capsule_collision_shape.h"
#include "physics/bullet/bullet_heightmap_collision_shape.h"
#include "physics/bullet/bullet_mesh_collision_shape.h"
#include "physics/collision_shape.h"
#include "physics/rigid_body.h"

namespace iris
{

/**
 * Class for rendering bullet physics debug geometry to a scene.
 *
 * Whilst bullet does provide a btIDebugDraw interface it's designed around immediate mode rendering i.e. each update
 * it provides a stream of lines to draw. Forcing this into the design of the engine is cumbersome and slow. Instead
 * we expose methods for registering bullet objects we want to debug draw and internally use a btIDebugDraw to generate
 * the debug lines These are then converted to a Mesh which we add to a Scene and can transform as normal.
 */

class DebugDraw
{
  public:
    /**
     * Construct a new DebugDraw.
     */
    DebugDraw();

    /**
     * Update all the registered rigid bodies.
     */
    void update();

    /**
     * Set the scene to render to.
     *
     * Note that if no scene is set the class will buffer all registered classes until a scene is set. This method
     * effectively enables debug drawing.
     *
     * @param scene
     *   Scene to render debug geometry to.
     */
    void set_scene(Scene *scene);

    /**
     * Register a new BoxCollisionShape, this allows any rigid body registered with this shape to be debug drawn.
     *
     * @param collision_shape
     *   The collision shape object to register.
     */
    void register_box_collision_shape(const BulletBoxCollisionShape *collision_shape);

    /**
     * Register a new CapsuleCollisionShape, this allows any rigid body registered with this shape to be debug drawn.
     *
     * @param collision_shape
     *   The collision shape object to register.
     */
    void register_capsule_collision_shape(const BulletCapsuleCollisionShape *collision_shape);

    /**
     * Register a new MeshCollisionShape, this allows any rigid body registered with this shape to be debug drawn.
     *
     * @param collision_shape
     *   The collision shape object to register.
     */
    void register_mesh_collision_shape(const BulletMeshCollisionShape *collision_shape);

    /**
     * Register a new HeightMapCollisionShape, this allows any rigid body registered with this shape to be debug drawn.
     *
     * @param collision_shape
     *   The collision shape object to register.
     */
    void register_height_map_collision_shape(const BulletHeightmapCollisionShape *collision_shape);

    /**
     * Register a new RigidBody for debug drawing.
     *
     * @param rigid_body
     *   The rigid body to register.
     */
    void register_rigid_body(const RigidBody *rigid_body);

    /**
     * Deregister a new RigidBody for debug drawing.
     *
     * @param rigid_body
     *   The rigid body to deregister.
     */
    void deregister_rigid_body(const RigidBody *rigid_body);

  private:
    /**
     * Implementation of btIDebugDraw which allows us to get debug geometry. This works because the base implementation
     * of all the debug methods call drawLine, which we override to store the drawn line.
     */
    struct BulletDebugDraw : btIDebugDraw
    {
        void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &colour) override
        {
            vertices_.emplace_back(
                Vector3{from.x(), from.y(), from.z()},
                Colour{colour.x(), colour.y(), colour.z()},
                Vector3{to.x(), to.y(), to.z()},
                Colour{colour.x(), colour.y(), colour.z()});
        }

        ~BulletDebugDraw() override = default;

        void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) override
        {
            throw Exception("unimplemented");
        }

        void reportErrorWarning(const char *) override
        {
            throw Exception("unimplemented");
        }

        void draw3dText(const ::btVector3 &, const char *) override
        {
            throw Exception("unimplemented");
        }

        void setDebugMode(int) override
        {
        }

        int getDebugMode() const override
        {
            return 0;
        }

        /**
         * Flush all the previously drawn lines to a Mesh.
         *
         * @returns
         *   A Mesh containing all the previously drawn lines.
         */
        std::unique_ptr<Mesh> flush()
        {
            std::vector<VertexData> vertices{};
            std::vector<std::uint32_t> indices;

            // convert all drawn lines to vertex and index data
            for (const auto &[from_position, from_colour, to_position, to_colour] : vertices_)
            {
                vertices.emplace_back(from_position, Vector3{1.0f}, from_colour, Vector3{});
                indices.emplace_back(static_cast<std::uint32_t>(vertices.size() - 1u));

                vertices.emplace_back(to_position, Vector3{1.0f}, to_colour, Vector3{});
                indices.emplace_back(static_cast<std::uint32_t>(vertices.size() - 1u));
            }

            // clear the drawn line store for future calls
            vertices_.clear();

            return Root::mesh_manager().unique_mesh(vertices, indices);
        }

        /** Cached drawn lines from bullet. */
        std::vector<std::tuple<Vector3, Colour, Vector3, Colour>> vertices_;
    };

    /** Scene to render to debug geometry to. */
    Scene *scene_;

    /** Object used to generate bullet debug geometry. */
    BulletDebugDraw bullet_debug_draw_;

    /** Collection of engine mesh objects for registered collision shapes. */
    std::unordered_map<const CollisionShape *, std::unique_ptr<Mesh>> meshes_;

    /** Collection of engine entities for registered rigid bodies. */
    std::unordered_map<const RigidBody *, SingleEntity *> bodies_;
};
}
