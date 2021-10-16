////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_physics_manager.h"

#include <memory>

#include "physics/basic_character_controller.h"
#include "physics/bullet/bullet_physics_system.h"
#include "physics/bullet/bullet_rigid_body.h"
#include "physics/physics_manager.h"
#include "physics/rigid_body.h"

namespace iris
{

PhysicsSystem *BulletPhysicsManager::create_physics_system()
{
    physics_system_ = std::make_unique<BulletPhysicsSystem>();
    return current_physics_system();
}

PhysicsSystem *BulletPhysicsManager::current_physics_system()
{
    return physics_system_.get();
}

}
