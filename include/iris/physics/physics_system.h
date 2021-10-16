////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <memory>
#include <optional>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/character_controller.h"
#include "physics/collision_shape.h"
#include "physics/rigid_body.h"

namespace iris
{

class RenderEntity;

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
        CollisionShape *collision_shape,
        RigidBodyType type) = 0;

    /**
     * Create a CharacterController and add it to the simulation.
     *
     * @returns
     *   A pointer to the newly created CharacterController.
     */
    virtual CharacterController *create_character_controller() = 0;

    /**
     * Create a CollisionShape for a box.
     *
     * @param half_size
     *   The extends from the center of the box which define its size.
     *
     * @returns
     *   Pointer to newly created CollisionShape.
     */
    virtual CollisionShape *create_box_collision_shape(const Vector3 &half_size) = 0;

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
    virtual CollisionShape *create_capsule_collision_shape(float width, float height) = 0;

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
     * @param body
     *   Body to remove.
     */
    virtual void remove(CharacterController *charaacter) = 0;

    /**
     * Cast a ray into physics engine world.
     *
     * @param origin
     *   Origin of ray.
     *
     * @param direction.
     *   Direction of ray.
     *
     * @returns
     *   If ray hits an object then a tuple [object hit, point of intersection],
     *   else empty optional.
     */
    virtual std::optional<std::tuple<RigidBody *, Vector3>> ray_cast(const Vector3 &origin, const Vector3 &direction)
        const = 0;

    /**
     * Add a body to be excluded from ray_casts
     *
     * @param body
     *   Body to ignore.
     */
    virtual void ignore_in_raycast(RigidBody *body) = 0;

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
