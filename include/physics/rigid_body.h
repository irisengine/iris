#pragma once

#include <any>

#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris
{

/**
 * Interface for a rigid body. A rigid body is a physics entity that can be
 * added to the physics systems and simulated. It will collide and interact
 * with other rigid bodies.
 */
class RigidBody
{
  public:
    // default
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
     * @returns
     *   Linear velocity.
     */
    virtual Vector3 linear_velocity() const = 0;

    /**
     * Get angular velocity.
     *
     * @returns
     *   Angular velocity.
     */
    virtual Vector3 angular_velocity() const = 0;

    /**
     * Set linear velocity.
     *
     * @param linear_velocity
     *   New linear velocity.
     */
    virtual void set_linear_velocity(const Vector3 &linear_velocity) = 0;

    /**
     * Set angular velocity.
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
    virtual void reposition(
        const Vector3 &position,
        const Quaternion &orientation) = 0;

    /**
     * Get native handle for physics engine implementation of rigid body.
     *
     * @returns
     *   Physics engine native handle.
     */
    virtual std::any native_handle() const = 0;
};

}
