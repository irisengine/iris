////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris
{

class RigidBody;
class PhysicsSystem;

/**
 * Interface for a character controller. Deriving classes should use this interface to implement how a character should
 * move for their game.
 */
class CharacterController
{
  public:
    virtual ~CharacterController() = default;

    virtual void set_movement_direction(const Vector3 &direction) = 0;

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
     * Set speed of character.
     *
     * @param speed
     *   New speed.
     */
    virtual void set_speed(float speed) = 0;

    /**
     * Reposition character.
     *
     * @param position
     *   New position.
     *
     * @param orientation
     *   New orientation.
     */
    virtual void reposition(const Vector3 &position, const Quaternion &orientation) = 0;

    /**
     * Get the underlying RigidBody.
     *
     * @returns
     *   Underlying RigidBody.
     */
    virtual RigidBody *rigid_body() const = 0;

    virtual void update(PhysicsSystem *ps, std::chrono::milliseconds delta) = 0;
};

}
