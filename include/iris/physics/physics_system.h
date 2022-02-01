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

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/character_controller.h"
#include "physics/collision_shape.h"
#include "physics/contact_point.h"
#include "physics/ray_cast_result.h"
#include "physics/rigid_body.h"

namespace iris
{

class RenderEntity;
class Mesh;

/**
 * Interface for a class which stores the current state of a PhysicsSystem.
 */
struct PhysicsState
{
    virtual ~PhysicsState() = default;
};

/**
 * Interface for a class which can manage and simulate a physics world.
 */
class PhysicsSystem
{
  public:
    PhysicsSystem() = default;

    virtual ~PhysicsSystem() = default;

    // disabled
    PhysicsSystem(const PhysicsSystem &) = delete;
    PhysicsSystem &operator=(const PhysicsSystem &) = delete;

    /**
     * Step the physics system by the supplied time.
     *
     * @param time_step
     *   The amount of time to simulate.
     */
    virtual void step(std::chrono::milliseconds time_step) = 0;

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
    virtual RigidBody *create_rigid_body(
        const Vector3 &position,
        const CollisionShape *collision_shape,
        RigidBodyType type) = 0;

    /**
     * Create a CollisionShape for a box.
     *
     * @param half_size
     *   The extends from the center of the box which define its size.
     *
     * @returns
     *   Pointer to newly created CollisionShape.
     */
    virtual const CollisionShape *create_box_collision_shape(const Vector3 &half_size) = 0;

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
    virtual const CollisionShape *create_capsule_collision_shape(float width, float height) = 0;

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
    virtual const CollisionShape *create_mesh_collision_shape(const Mesh *mesh, const Vector3 &scale) = 0;

    /**
     * Add a character controller.
     *
     * @param character_controller
     *   Character controller to add.
     *
     * @return
     *   Pointer to added character controller.
     */
    virtual CharacterController *add(std::unique_ptr<CharacterController> character_controller) = 0;

    /**
     * Create a CharacterController object.
     *
     * @param args
     *   Arguments to forward to CharacterController constructor.
     *
     * @returns
     *   Pointer to newly added CharacterController.
     */
    template <class T, class... Args>
    T *create_character_controller(Args &&...args)
    {
        return static_cast<T *>(add(std::make_unique<T>(std::forward<Args>(args)...)));
    }

    /**
     * Remove a body from the physics system.
     *
     * This will release all resources for the body, using the handle after this
     * call is undefined.
     *
     * @param body
     *   Body to remove.
     */
    virtual void remove(RigidBody *body) = 0;

    /**
     * Character controller a body from the physics system.
     *
     * This will release all resources for the character, using the handle after
     * this call is undefined.
     *
     * @param character
     *   Character to remove.
     */
    virtual void remove(CharacterController *character) = 0;

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
    virtual std::vector<RayCastResult> ray_cast(
        const Vector3 &origin,
        const Vector3 &direction,
        const std::set<const RigidBody *> &ignore) = 0;

    /**
     * Query all contacts with a body.
     *
     * @param body
     *   The body to test, note that this will be contact_a in all the returned ContactPoint objects.
     *
     * @returns
     *   Collection of ContactPoint objects for all bodies colliding with body.
     */
    virtual std::vector<ContactPoint> contacts(RigidBody *body) = 0;

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
    virtual std::unique_ptr<PhysicsState> save() = 0;

    /**
     * Load saved state. This will restore the simulation to that of the
     * supplied state.
     *
     * See save() comments for details of limitations.
     *
     * @param state
     *   State to restore from.
     */
    virtual void load(const PhysicsState *state) = 0;

    /**
     * Enable debug rendering. This should only be called once.
     *
     * @param entity.
     *   The RenderEntity to store debug render data in.
     */
    virtual void enable_debug_draw(RenderEntity *entity) = 0;
};

}
