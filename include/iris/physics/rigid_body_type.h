#pragma once

#include <cstdint>

namespace iris
{

/**
 * Enumeration of possible rigid body types.
 */
enum class RigidBodyType : std::uint32_t
{
    /**
     * A rigid body that is simulated as part of the physics engine and
     * approximates a real world object (forces, velocity, collision, etc.).
     */
    NORMAL,

    /**
     * A rigid body that does not move. Other non static rigid bodies will
     * collide with it.
     */
    STATIC,

    /**
     * A rigid body that can be moved and detects collision, although it does
     * not perform collision resolution.
     */
    GHOST
};

}
