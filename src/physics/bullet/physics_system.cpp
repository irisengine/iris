#include "physics/physics_system.h"

#include <memory>
#include <vector>

#include <btBulletDynamicsCommon.h>

namespace eng
{

struct PhysicsSystem::implementation
{
    std::unique_ptr<::btDefaultCollisionConfiguration> collision_config;
    std::unique_ptr<::btCollisionDispatcher> collision_dispatcher;
    std::unique_ptr<::btBroadphaseInterface> broadphase;
    std::unique_ptr<::btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<::btDiscreteDynamicsWorld> world;
    std::vector<std::unique_ptr<RigidBody>> bodies;
    std::vector<std::unique_ptr<CharacterController>> character_controllers;
};

PhysicsSystem::PhysicsSystem()
    : impl_(std::make_unique<implementation>())
{
    impl_->collision_config = std::make_unique<::btDefaultCollisionConfiguration>();
    impl_->collision_dispatcher = std::make_unique<::btCollisionDispatcher>(impl_->collision_config.get());
    impl_->broadphase = std::make_unique<::btDbvtBroadphase>();
    impl_->solver = std::make_unique<::btSequentialImpulseConstraintSolver>();
    impl_->world = std::make_unique<::btDiscreteDynamicsWorld>(
        impl_->collision_dispatcher.get(),
        impl_->broadphase.get(),
        impl_->solver.get(),
        impl_->collision_config.get());

    impl_->world->setGravity({ 0.0f, -10.0f, 0.0f });
}

PhysicsSystem::~PhysicsSystem() = default;

void PhysicsSystem::step(std::chrono::milliseconds time_step)
{
    const auto ticks = static_cast<float>(time_step.count());
    impl_->world->stepSimulation(ticks / 1000.0f, 1);
}

RigidBody* PhysicsSystem::add(std::unique_ptr<RigidBody> body)
{
    impl_->bodies.emplace_back(std::move(body));

    auto *bullet_body = std::any_cast<::btRigidBody*>(
        impl_->bodies.back()->native_handle());
    impl_->world->addRigidBody(bullet_body);

    return impl_->bodies.back().get();
}

CharacterController* PhysicsSystem::add(std::unique_ptr<CharacterController> character)
{
    impl_->character_controllers.emplace_back(std::move(character));

    auto *bullet_body = std::any_cast<::btRigidBody*>(
        impl_->character_controllers.back()->native_handle());
    impl_->world->addRigidBody(bullet_body);

    return impl_->character_controllers.back().get();
}

std::optional<Vector3> PhysicsSystem::ray_cast(
    const Vector3 &origin,
    const Vector3 &direction) const
{
    std::optional<Vector3> hit;

    // bullet does ray tracing between two vectors, so we create an end vector
    // some great distance away
    ::btVector3 from{ origin.x, origin.y, origin.z };
    const auto far_away = origin + (direction * 10000.0f);
    ::btVector3 to{ far_away.x, far_away.y, far_away.z };

    ::btCollisionWorld::ClosestRayResultCallback callback{ from, to };

    impl_->world->rayTest(from, to, callback);

    if(callback.hasHit())
    {
        const auto hit_position = callback.m_hitPointWorld;
        hit = { hit_position.x(), hit_position.y(), hit_position.z() };
    }

    return hit;
}

}

