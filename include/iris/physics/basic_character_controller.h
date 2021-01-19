#pragma once

#include <any>
#include <memory>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/character_controller.h"
#include "physics/rigid_body.h"

namespace iris
{

class PhysicsSystem;

/**
 * Implementation of CharacterController for a basic FPS character controller.
 * Uses a capsule shape for character.
 */
class BasicCharacterController : public CharacterController
{
  public:
    /**
     * Create a BasicCharacterController.
     *
     * @param physics_system
     *   Pointer to physics_system that owns this controller.
     */
    explicit BasicCharacterController(PhysicsSystem *physics_system);

    /**
     * Destructor.
     */
    ~BasicCharacterController() override;

    /**
     * Set the direction the character is walking. Should be a normalised
     * vector.
     *
     * @param direction
     *   Direction character is moving.
     */
    void set_walk_direction(const Vector3 &direction) override;

    /**
     * Get position of character in the world.
     *
     * @returns
     *   World coordinates of character.
     */
    Vector3 position() const override;

    /**
     * Get orientation of character.
     *
     * @returns
     *   Orientation of character
     */
    Quaternion orientation() const override;

    /**
     * Get linear velocity.
     *
     * @returns
     *   Linear velocity.
     */
    Vector3 linear_velocity() const override;

    /**
     * Get angular velocity.
     *
     * @returns
     *   Angular velocity.
     */
    Vector3 angular_velocity() const override;

    /**
     * Set linear velocity.
     *
     * @param linear_velocity
     *   New linear velocity.
     */
    void set_linear_velocity(const Vector3 &linear_velocity) override;

    /**
     * Set angular velocity.
     *
     * @param angular_velocity
     *   New angular velocity.
     */
    void set_angular_velocity(const Vector3 &angular_velocity) override;

    /**
     * Set speed of character.
     *
     * @param speed
     *   New speed.
     */
    void set_speed(float speed) override;

    /**
     * Reposition character.
     *
     * @param position
     *   New position.
     *
     * @param orientation
     *   New orientation.
     */
    void reposition(const Vector3 &position, const Quaternion &orientation)
        override;

    /**
     * Make the character jump.
     */
    void jump() override;

    /**
     * Check if character is standing on the ground.
     *
     * @returns
     *   True if character is on a surface, false otherwise.
     */
    bool on_ground() const override;

    /**
     * Get the underlying RigidBody.
     *
     * @returns
     *   Underlying RigidBody.
     */
    RigidBody *rigid_body() const override;

    /**
     * Set the collision shape for the controller.
     *
     * @param collision_shape
     *   New collision shape.
     */
    void set_collision_shape(
        std::unique_ptr<CollisionShape> collision_shape) override;

  private:
    /** Speed of character. */
    float speed_;

    /* Mass of character. */
    float mass_;

    /** Physics system. */
    PhysicsSystem *physics_system_;

    /** Underlying rigid body, */
    std::unique_ptr<RigidBody> body_;
};

}
