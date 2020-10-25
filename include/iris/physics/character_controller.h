#pragma once

#include <any>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/collision_shape.h"

namespace iris
{

class RigidBody;

/**
 * Interface for a character controller. Deriving classes should use this
 * interface to implement how a character should move for their game.
 */
class CharacterController
{
  public:
    virtual ~CharacterController() = default;

    /**
     * Set the direction the character is walking. Should be a normalised
     * vector.
     *
     * @param direction
     *   Direction character is moving.
     */
    virtual void set_walk_direction(const Vector3 &direction) = 0;

    /**
     * Get position of character in the world.
     *
     * @returns
     *   World coordinates of character.
     */
    virtual Vector3 position() const = 0;

    /**
     * Get orientation of character.
     *
     * @returns
     *   Orientation of character
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
     * Set speed of character.
     *
     * @param speed
     *   New speed.
     */
    virtual void set_speed(real speed) = 0;

    /**
     * Reposition character.
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
     * Make the character jump.
     */
    virtual void jump() = 0;

    /**
     * Get native handle for physics engine implementation of internal rigid
     * body.
     *
     * @returns
     *   Physics engine native handle.
     */
    virtual std::any native_handle() const = 0;

    /**
     * Check if character is standing on the ground.
     *
     * @returns
     *   True if character is on a surface, false otherwise.
     */
    virtual bool on_ground() const = 0;

    virtual RigidBody *rigid_body() const = 0;

    virtual void set_collision_shape(
        std::unique_ptr<CollisionShape> collision_shape) = 0;
};

}
