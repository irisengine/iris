////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <set>
#include <vector>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btMotionState.h>
#include <btBulletDynamicsCommon.h>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/bullet/bullet_collision_shape.h"
#include "physics/bullet/debug_draw.h"
#include "physics/character_controller.h"
#include "physics/collision_shape.h"
#include "physics/contact_point.h"
#include "physics/physics_system.h"
#include "physics/ray_cast_result.h"
#include "physics/rigid_body.h"

namespace iris
{

class Mesh;

/**
 * Implementation of PhysicsSystem for bullet.
 */
class BulletPhysicsSystem : public PhysicsSystem
{
  public:
    /**
     * Construct a new BulletPhysicsSystem.
     */
    BulletPhysicsSystem();

    ~BulletPhysicsSystem() override;

    // disabled
    BulletPhysicsSystem(const BulletPhysicsSystem &) = delete;
    BulletPhysicsSystem &operator=(const BulletPhysicsSystem &) = delete;

    /**
     * Step the physics system by the supplied time.
     *
     * @param time_step
     *   The amount of time to simulate.
     */
    void step(std::chrono::milliseconds time_step) override;

    /**
     * Create a RigidBody and add it to the simulation.
     *
     * @param position
     *   Position in world space.
     *
     * @param collision_shape
     *   The shape that defined the rigid body, this is used for collision
     *   detection/response.
     *
     * @param type
     *   The type of rigid body, this effects how this body interacts with
     *   others.
     *
     * @returns
     *   A pointer to the newly created RigidBody.
     */

    /**
     * Create a CharacterController and add it to the simulation.
     *
     * @returns
     *   A pointer to the newly created CharacterController.
     */
    CharacterController *create_character_controller() override;
    RigidBody *create_rigid_body(const Vector3 &position, const CollisionShape *collision_shape, RigidBodyType type)
        override;

    /**
     * Create a CollisionShape for a box.
     *
     * @param half_size
     *   The extends from the center of the box which define its size.
     *
     * @returns
     *   Pointer to newly created CollisionShape.
     */
    const CollisionShape *create_box_collision_shape(const Vector3 &half_size) override;

    /**
     * Create a CollisionShape for a capsule.
     *
     * @param width
     *   Diameter of capsule.
     *
     * @param height
     *   Height of capsule.
     *
     * @returns
     *   Pointer to newly created CollisionShape.
     */
    const CollisionShape *create_capsule_collision_shape(float width, float height) override;

    /**
     * Create a CollisionShape from a Mesh.
     *
     * @param mesh
     *   Mesh to create collision shape from.
     *
     * @param scale
     *   Scale of mesh as it will be rendered.
     *
     * @returns
     *   Pointer to newly created CollisionShape.
     */
    const CollisionShape *create_mesh_collision_shape(const Mesh *mesh, const Vector3 &scale) override;

    /**
     * Remove a body from the physics system.
     *
     * This will release all resources for the body, using the handle after this
     * call is undefined.
     *
     * @param body
     *   Body to remove.
     */
    void remove(RigidBody *body) override;

    /**
     * Character controller a body from the physics system.
     *
     * This will release all resources for the character, using the handle after
     * this call is undefined.
     *
     * @param body
     *   Body to remove.
     */
    void remove(CharacterController *charaacter) override;

    /**
     * Cast a ray into physics engine world and get all hits.
     *
     * @param origin
     *   Origin of ray.
     *
     * @param direction.
     *   Direction of ray.
     *
     * @param ignore
     *   Collection of rigid bodies that should be ignored from ray cast results.
     *
     * @returns
     *   Collection of RayCastResult objects for all intersection with ray. These will be sorted from distance to origin
     *   (closest first).
     */
    std::vector<RayCastResult> ray_cast(
        const Vector3 &origin,
        const Vector3 &direction,
        const std::set<const RigidBody *> &ignore) override;

    /**
     * Query all contacts with a body.
     *
     * @param body
     *   The body to test, note that this will be contact_a in all the returned ContactPoint objects.
     *
     * @returns
     *   Collection of ContactPoint objects for all bodies colliding with body.
     */
    std::vector<ContactPoint> contacts(RigidBody *body) override;

    /**
     * Save the current state of the simulation.
     *
     * Note that depending on the implementation this may be a "best guess"
     * state save. Restoring isn't guaranteed to produce identical results
     * although it should be close enough.
     *
     * @returns
     *   Saved state.
     */
    std::unique_ptr<PhysicsState> save() override;

    /**
     * Load saved state. This will restore the simulation to that of the
     * supplied state.
     *
     * See save() comments for details of limitations.
     *
     * @param state
     *   State to restore from.
     */
    void load(const PhysicsState *state) override;

    /**
     * Enable debug rendering. This should only be called once.
     *
     * @param entity.
     *   The RenderEntity to store debug render data in.
     */
    void enable_debug_draw(RenderEntity *entity) override;

  private:
    /** Bullet interface for detecting AABB overlapping pairs. */
    std::unique_ptr<btBroadphaseInterface> broadphase_;

    /**
     * Bullet callback for adding and removing overlapping pairs from the
     * broadphase.
     */
    std::unique_ptr<btGhostPairCallback> ghost_pair_callback_;

    /** Bullet collision config object, */
    std::unique_ptr<btDefaultCollisionConfiguration> collision_config_;

    /** Object containing algorithms for handling collision pairs. */
    std::unique_ptr<btCollisionDispatcher> collision_dispatcher_;

    /** Bullet constraint solver. */
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;

    /** Bullet simulated world. */
    std::unique_ptr<btDiscreteDynamicsWorld> world_;

    /** Collection of rigid bodies. */
    std::vector<std::unique_ptr<RigidBody>> bodies_;

    /** Collection of character controllers. */
    std::vector<std::unique_ptr<CharacterController>> character_controllers_;

    /** DebugDraw object. */
    std::unique_ptr<DebugDraw> debug_draw_;

    /** Collection of collision shapes. */
    std::vector<std::unique_ptr<BulletCollisionShape>> collision_shapes_;

    /** To prevent overloading the rendering with debug data we only update the debug geometry at a fixed interval. */
    std::chrono::system_clock::time_point next_debug_update_;
};

}
