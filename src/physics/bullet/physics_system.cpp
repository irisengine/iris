#include "physics/physics_system.h"

#include <chrono>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btMotionState.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <btBulletDynamicsCommon.h>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "log/log.h"
#include "physics/bullet/debug_draw.h"
#include "physics/character_controller.h"
#include "physics/rigid_body.h"

namespace iris
{

/**
 * Saved information about a rigid body. Used in PhysicsState.
 */
struct RigidBodyState
{
    RigidBodyState(
        const ::btTransform &transform,
        const ::btVector3 &linear_velocity,
        const ::btVector3 &angular_velocity)
        : transform(transform)
        , linear_velocity(linear_velocity)
        , angular_velocity(angular_velocity)
    {
    }

    ::btTransform transform;
    ::btVector3 linear_velocity;
    ::btVector3 angular_velocity;
};

/**
 * Struct for saving the state of the physics simulation. It simply stores
 * RigidBodyState for all rigid bodies. Note that collision information is
 * *not* saved.
 */
struct PhysicsState
{
    std::map<::btRigidBody *, RigidBodyState> bodies;
};

/**
 * Custom delete for PhysicsState. Simply deletes allocated memory.
 */
void PhysicsStateDeleter::operator()(PhysicsState *state)
{
    if (state != nullptr)
    {
        delete state;
    }
}

struct PhysicsSystem::implementation
{
    std::unique_ptr<::btDefaultCollisionConfiguration> collision_config;
    std::unique_ptr<::btCollisionDispatcher> collision_dispatcher;
    std::unique_ptr<::btBroadphaseInterface> broadphase;
    std::unique_ptr<::btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<::btDiscreteDynamicsWorld> world;
    std::unique_ptr<::btGhostPairCallback> ghost_pair_callback;
    std::vector<std::unique_ptr<RigidBody>> bodies;
    std::vector<std::unique_ptr<CharacterController>> character_controllers;
    DebugDraw debug_draw;
};

PhysicsSystem::PhysicsSystem()
    : impl_(std::make_unique<implementation>())
    , draw_debug_(false)
{
    impl_->collision_config =
        std::make_unique<::btDefaultCollisionConfiguration>();
    impl_->collision_dispatcher = std::make_unique<::btCollisionDispatcher>(
        impl_->collision_config.get());
    impl_->broadphase = std::make_unique<::btDbvtBroadphase>();
    impl_->solver = std::make_unique<::btSequentialImpulseConstraintSolver>();
    impl_->world = std::make_unique<::btDiscreteDynamicsWorld>(
        impl_->collision_dispatcher.get(),
        impl_->broadphase.get(),
        impl_->solver.get(),
        impl_->collision_config.get());
    impl_->ghost_pair_callback = std::make_unique<::btGhostPairCallback>();
    impl_->broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(
        impl_->ghost_pair_callback.get());

    impl_->world->setGravity({0.0f, -10.0f, 0.0f});

    // create debug drawer, only draw wireframe as that's what we support
    impl_->debug_draw.setDebugMode(::btIDebugDraw::DBG_DrawWireframe);
    impl_->world->setDebugDrawer(&impl_->debug_draw);
}

PhysicsSystem::~PhysicsSystem()
{
    try
    {

    }
        for (const auto &body : impl_->bodies)
        {
            if (body->type() == RigidBodyType::GHOST)
            {
                auto *bullet_ghost =
                    std::any_cast<::btGhostObject *>(body->native_handle());
                impl_->world->removeCollisionObject(bullet_ghost);
            }
            else
            {
                auto *bullet_body =
                    std::any_cast<::btRigidBody *>(body->native_handle());
                impl_->world->removeRigidBody(bullet_body);
            }
        }
    }
    catch (...)
    {
        LOG_ERROR("physics_system", "exception caught during dtor");
    }
}

void PhysicsSystem::step(std::chrono::milliseconds time_step)
{
    const auto ticks = static_cast<float>(time_step.count());
    impl_->world->stepSimulation(ticks / 1000.0f, 1);

    if (draw_debug_)
    {
        // tell bullet to draw debug world
        impl_->world->debugDrawWorld();

        // now we pass bullet debug information to our render system
        impl_->debug_draw.render();
    }
}

RigidBody *PhysicsSystem::add(std::unique_ptr<RigidBody> body)
{
    impl_->bodies.emplace_back(std::move(body));
    auto *b = impl_->bodies.back().get();

    if (b->type() == RigidBodyType::GHOST)
    {
        auto *bullet_ghost =
            std::any_cast<::btGhostObject *>(b->native_handle());
        impl_->world->addCollisionObject(bullet_ghost);
    }
    else
    {
        auto *bullet_body = std::any_cast<::btRigidBody *>(b->native_handle());
        impl_->world->addRigidBody(bullet_body);
    }

    return b;
}

CharacterController *PhysicsSystem::add(
    std::unique_ptr<CharacterController> character)
{
    impl_->character_controllers.emplace_back(std::move(character));

    auto *bullet_body = std::any_cast<::btRigidBody *>(
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
    ::btVector3 from{origin.x, origin.y, origin.z};
    const auto far_away = origin + (direction * 10000.0f);
    ::btVector3 to{far_away.x, far_away.y, far_away.z};

    ::btCollisionWorld::ClosestRayResultCallback callback{from, to};

    impl_->world->rayTest(from, to, callback);

    if (callback.hasHit())
    {
        const auto hit_position = callback.m_hitPointWorld;
        hit = {hit_position.x(), hit_position.y(), hit_position.z()};
    }

    return hit;
}

std::unique_ptr<PhysicsState, PhysicsStateDeleter> PhysicsSystem::save()
{
    std::unique_ptr<PhysicsState, PhysicsStateDeleter> state(new PhysicsState);

    // save data for all rigid bodies
    for (const auto &body : impl_->bodies)
    {
        auto *bullet_body =
            std::any_cast<::btRigidBody *>(body->native_handle());

        state->bodies.try_emplace(
            bullet_body,
            bullet_body->getWorldTransform(),
            bullet_body->getLinearVelocity(),
            bullet_body->getAngularVelocity());
    }

    // save data for all character controllers
    for (const auto &character : impl_->character_controllers)
    {
        auto *bullet_body =
            std::any_cast<::btRigidBody *>(character->native_handle());

        state->bodies.try_emplace(
            bullet_body,
            bullet_body->getWorldTransform(),
            bullet_body->getLinearVelocity(),
            bullet_body->getAngularVelocity());
    }

    return state;
}

void PhysicsSystem::load(const PhysicsState *state)
{
    // restore state for each rigid body
    for (const auto &[bullet_body, body_state] : state->bodies)
    {
        bullet_body->clearForces();

        bullet_body->setWorldTransform(body_state.transform);
        bullet_body->setCenterOfMassTransform(body_state.transform);
        bullet_body->setLinearVelocity(body_state.linear_velocity);
        bullet_body->setAngularVelocity(body_state.angular_velocity);
    }
}

void PhysicsSystem::set_draw_debug(bool draw_debug)
{
    draw_debug_ = draw_debug;
}

}
