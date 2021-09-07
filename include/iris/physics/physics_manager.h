#pragma once

namespace iris
{

class PhysicsSystem;

/**
 * Interface for a class which creates PhysicsSystem objects.
 */
class PhysicsManager
{
  public:
    virtual ~PhysicsManager() = default;

    /**
     * Create a new PhysicsSystem.
     */
    virtual PhysicsSystem *create_physics_system() = 0;

    /**
     * Get the currently active PhysicsSystem.
     *
     * @returns
     *   Pointer to the current PhysicsSystem, nullptr if one does not exist.
     */
    virtual PhysicsSystem *current_physics_system() = 0;
};

}
