#pragma once

#include <any>
#include <memory>

#include "core/real.h"
#include "core/vector3.h"
#include "physics/character_controller.h"

namespace eng
{

/**
 * Implementation of CharacterController for a basic FPS character controller.
 * Uses a capsule shape for character.
 */
class BasicCharacterController : public CharacterController
{
    public:

        /**
         * Create a BasicCharacterController.
         */
        BasicCharacterController();

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
         * Make the character jump.
         */
        void jump() override;

        /**
         * Get native handle for physics engine implementation of internal rigid
         * body.
         *
         * @returns
         *   Physics engine native handle.
         */
        std::any native_handle() const override;

        /**
         * Check if character is standing on the ground.
         *
         * @returns
         *   True if character is on a surface, false otherwise.
         */
        bool on_ground() const override;

    private:

        /**
         * Speed of character.
         */
        real speed_;

        /**
         * Mass of character.
         */
        real mass_;

        /**
         * Physics API implementation.
         */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

