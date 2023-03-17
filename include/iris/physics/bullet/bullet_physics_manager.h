////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

#include "graphics/mesh_manager.h"
#include "physics/bullet/bullet_physics_system.h"
#include "physics/physics_manager.h"

namespace iris
{

/**
 * Implementation of PhysicsManager for bullet.
 */
class BulletPhysicsManager : public PhysicsManager
{
  public:
    /**
     * Create new BulletPhysicsManager object.
     *
     * @param mesh_manager
     *   Mesh manager object.
     */
    BulletPhysicsManager(MeshManager &mesh_manager);
    ~BulletPhysicsManager() override = default;

    /**
     * Create a new PhysicsSystem.
     */
    PhysicsSystem *create_physics_system() override;

    /**
     * Get the currently active PhysicsSystem.
     *
     * @returns
     *   Pointer to the current PhysicsSystem, nullptr if one does not exist.
     */
    PhysicsSystem *current_physics_system() override;

  private:
    /** Current physics system. */
    std::unique_ptr<BulletPhysicsSystem> physics_system_;

    /** Mesh manager object. */
    MeshManager &mesh_manager_;
};

}
