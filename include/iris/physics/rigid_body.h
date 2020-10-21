#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/rigid_body_type.h"

namespace iris
{

class CollisionShape;

/**
 * A rigid body is a physics entity that can be added to the physics systems and
 * simulated. It can collide and interact with other rigid bodies.
 */
class RigidBody
{
  public:
    /**
     * Create a new rigid body.
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
     */
    RigidBody(
        const Vector3 &position,
        std::unique_ptr<CollisionShape> collision_shape,
        RigidBodyType type);

    ~RigidBody();

    /**
     * Get position of rigid body centre of mass.
     *
     * @returns
     *   Rigid body position.
     */
    Vector3 position() const;

    /**
     * Get orientation of rigid body.
     *
     * @returns
     *   Rigid body orientation.
     */
    Quaternion orientation() const;

    /**
     * Get linear velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @returns
     *   Linear velocity.
     */
    Vector3 linear_velocity() const;

    /**
     * Get angular velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @returns
     *   Angular velocity.
     */
    Vector3 angular_velocity() const;

    /**
     * Set linear velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @param linear_velocity
     *   New linear velocity.
     */
    void set_linear_velocity(const Vector3 &linear_velocity);

    /**
     * Set angular velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @param angular_velocity
     *   New angular velocity.
     */
    void set_angular_velocity(const Vector3 &angular_velocity);

    /**
     * Reposition rigid body.
     *
     * @param position
     *   New position.
     *
     * @param orientation
     *   New orientation.
     */
    void reposition(const Vector3 &position, const Quaternion &orientation);

    /**
     * Get native handle for physics engine implementation of rigid body.
     *
     * @returns
     *   Physics engine native handle.
     */
    std::any native_handle() const;

    /**
     * Get the name of the rigid body. This is an optional trait and will return
     * an empty string if a name has not already been set.
     *
     * @returns
     *   Optional name of rigid body.
     */
    std::string name() const;

    /**
     * Set name.
     *
     * @param name
     *   New name.
     */
    void set_name(const std::string &name);

    /**
     * Get type.
     *
     * @returns
     *   Type of rigid body.
     */
    RigidBodyType type() const;

    /**
     * Pointer to collision shape.
     *
     * @returns
     *   Collision shape.
     */
    CollisionShape *collision_shape() const;

  private:
    /** Name of rigid body.*/
    std::string name_;

    /** Type of rigid body. */
    RigidBodyType type_;

    /** Collision shape of rigid body. */
    std::unique_ptr<CollisionShape> collision_shape_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
