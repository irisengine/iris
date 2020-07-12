#pragma once

#include <any>

#include "core/vector3.h"

namespace eng
{

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
};

}

