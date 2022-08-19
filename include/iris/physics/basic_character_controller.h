////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/character_controller.h"
#include "physics/physics_system.h"
#include "physics/rigid_body.h"

namespace iris
{

class PhysicsSystem;

/**
 * Implementation of CharacterController for a basic walking character controller.
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
    BasicCharacterController(PhysicsSystem *physics_system, float speed, float width, float height, float float_height);

    /**
     * Set the direction the character is walking. Should be a normalised
     * vector.
     *
     * @param direction
     *   Direction character is moving.
     */
    void set_movement_direction(const Vector3 &direction) override;

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
    void reposition(const Vector3 &position, const Quaternion &orientation) override;

    /**
     * Get the underlying RigidBody.
     *
     * @returns
     *   Underlying RigidBody.
     */
    RigidBody *rigid_body() const override;

    void update(PhysicsSystem *ps, std::chrono::milliseconds delta) override;

  protected:
    Vector3 movement_direction_;

    /** Speed of character. */
    float speed_;

    float float_height_;

    /** Underlying rigid body, */
    RigidBody *body_;
};

}
