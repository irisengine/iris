#pragma once

#include <memory>
#include <vector>

#include "rigid_body.hpp"

namespace eng
{

/**
 * Class for running the physics engine.
 */
class physics_system final
{
    public:

        /** Default */
        physics_system() = default;
        ~physics_system() = default;
        physics_system(physics_system&&) = default;
        physics_system& operator=(physics_system&&) = default;

        /** Disabled */
        physics_system(const physics_system&) = delete;
        physics_system& operator=(const physics_system&) = delete;

        /**
         * Add a rigid body to the engine.
         *
         * @param body
         *   Body to add.
         */
        void add(std::shared_ptr<rigid_body> body);

        /**
         * Perform a single step of the engine advancing the simulation by
         * the specified time.
         *
         * @param delta
         *   Amount of time to advance the simulation.
         */
        void step(const float delta) const;

    private:

        /** Collection of bodies to simulate. */
        std::vector<std::shared_ptr<rigid_body>> bodies_;

};

}

