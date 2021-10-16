////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

#include "core/vector3.h"
#include "physics/basic_character_controller.h"
#include "physics/bullet/bullet_physics_system.h"
#include "physics/collision_shape.h"
#include "physics/physics_manager.h"
#include "physics/rigid_body.h"
#include "physics/rigid_body_type.h"

namespace iris
{

/**
 * Implementation of PhysicsManager for bullet.
 */
class BulletPhysicsManager : public PhysicsManager
{
  public:
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
};

}
