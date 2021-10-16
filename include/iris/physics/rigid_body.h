////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/collision_shape.h"
#include "physics/rigid_body_type.h"

namespace iris
{

/**
 * Interface for a  rigid body, a physics entity that can be added to the
 * physics systems and simulated. It can collide and interact with other rigid
 * bodies.
 */
class RigidBody
{
  public:
    virtual ~RigidBody() = default;

    /**
     * Get position of rigid body centre of mass.
     *
     * @returns
     *   Rigid body position.
     */
    virtual Vector3 position() const = 0;

    /**
     * Get orientation of rigid body.
     *
     * @returns
     *   Rigid body orientation.
     */
    virtual Quaternion orientation() const = 0;

    /**
     * Get linear velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @returns
     *   Linear velocity.
     */
    virtual Vector3 linear_velocity() const = 0;

    /**
     * Get angular velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @returns
     *   Angular velocity.
     */
    virtual Vector3 angular_velocity() const = 0;

    /**
     * Set linear velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @param linear_velocity
     *   New linear velocity.
     */
    virtual void set_linear_velocity(const Vector3 &linear_velocity) = 0;

    /**
     * Set angular velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @param angular_velocity
     *   New angular velocity.
     */
    virtual void set_angular_velocity(const Vector3 &angular_velocity) = 0;

    /**
     * Reposition rigid body.
     *
     * @param position
     *   New position.
     *
     * @param orientation
     *   New orientation.
     */
    virtual void reposition(const Vector3 &position, const Quaternion &orientation) = 0;

    /**
     * Get the name of the rigid body. This is an optional trait and will return
     * an empty string if a name has not already been set.
     *
     * @returns
     *   Optional name of rigid body.
     */
    virtual std::string name() const = 0;

    /**
     * Set name.
     *
     * @param name
     *   New name.
     */
    virtual void set_name(const std::string &name) = 0;

    /**
     * Get type.
     *
     * @returns
     *   Type of rigid body.
     */
    virtual RigidBodyType type() const = 0;

    /**
     * Pointer to collision shape.
     *
     * @returns
     *   Collision shape.
     */
    virtual CollisionShape *collision_shape() const = 0;

    /**
     * Set collision shape.
     *
     * @param collision_shape
     *   New collision shape.
     */
    virtual void set_collision_shape(CollisionShape *collision_shape) = 0;

    /**
     * Apply an impulse (at the centre of mass).
     *
     * @param impulse
     *   Impulse to apply.
     */
    virtual void apply_impulse(const Vector3 &impulse) = 0;
};

}
