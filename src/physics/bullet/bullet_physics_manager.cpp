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

// RigidBody *BulletPhysicsManager::create_rigid_body(
//    const Vector3 &position,
//    std::unique_ptr<CollisionShape> collision_shape,
//    RigidBodyType type)
//{
//    auto rigid_body = std::make_unique<BulletRigidBody>(
//        position, std::move(collision_shape), type);
//    return physics_system_->add(std::move(rigid_body));
//}

// CharacterController
// *BulletPhysicsManager::create_basic_character_controller()
//{
//    return physics_system_
//        ->create_character_controller<BasicCharacterController>(
//            current_physics_system());
//}

}
