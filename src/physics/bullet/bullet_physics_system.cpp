////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_physics_system.h"

#include <chrono>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btMotionState.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <btBulletDynamicsCommon.h>

#include "core/error_handling.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "graphics/mesh_manager.h"
#include "log/log.h"
#include "physics/basic_character_controller.h"
#include "physics/bullet/bullet_box_collision_shape.h"
#include "physics/bullet/bullet_capsule_collision_shape.h"
#include "physics/bullet/bullet_collision_shape.h"
#include "physics/bullet/bullet_heightmap_collision_shape.h"
#include "physics/bullet/bullet_mesh_collision_shape.h"
#include "physics/bullet/bullet_rigid_body.h"
#include "physics/bullet/collision_callback.h"
#include "physics/bullet/debug_draw.h"
#include "physics/character_controller.h"
#include "physics/contact_point.h"
#include "physics/ray_cast_result.h"
#include "physics/rigid_body.h"

using namespace std::literals::chrono_literals;

namespace
{
/**
 * Helper function to remove a rigid body from a bullet dynamics world.
 *
 * @param body
 *   Body to remove.
 *
 * @param world
 *   World to remove from.
 */
void remove_body_from_world(iris::RigidBody *body, btDynamicsWorld *world)
{
    auto *bullet_body = static_cast<iris::BulletRigidBody *>(body);

    if ((body->type() == iris::RigidBodyType::GHOST) || (body->type() == iris::RigidBodyType::CHARACTER_CONTROLLER))
    {
        auto *bullet_ghost = static_cast<::btGhostObject *>(bullet_body->handle());
        world->removeCollisionObject(bullet_ghost);
    }
    else
    {
        auto *bullet_rigid = static_cast<::btRigidBody *>(bullet_body->handle());
        world->removeRigidBody(bullet_rigid);
    }
}
}

namespace iris
{

/**
 * Saved information about a rigid body. Used in PhysicsState.
 */
struct RigidBodyState
{
    RigidBodyState(const btTransform &transform, const btVector3 &linear_velocity, const btVector3 &angular_velocity)
        : transform(transform)
        , linear_velocity(linear_velocity)
        , angular_velocity(angular_velocity)
    {
    }

    btTransform transform;
    btVector3 linear_velocity;
    btVector3 angular_velocity;
};

/**
 * Struct for saving the state of the physics simulation. It simply stores
 * RigidBodyState for all rigid bodies. Note that collision information is
 * *not* saved.
 */
struct BulletPhysicsState : public PhysicsState
{
    ~BulletPhysicsState() override = default;

    std::map<btRigidBody *, RigidBodyState> bodies;
};

BulletPhysicsSystem::BulletPhysicsSystem(MeshManager &mesh_manager)
    : PhysicsSystem()
    , broadphase_(nullptr)
    , ghost_pair_callback_(nullptr)
    , collision_config_(nullptr)
    , collision_dispatcher_(nullptr)
    , solver_(nullptr)
    , world_(nullptr)
    , bodies_()
    , character_controllers_()
    , debug_draw_(mesh_manager)
    , collision_shapes_()
    , next_debug_update_(std::chrono::system_clock::now())
{
    collision_config_ = std::make_unique<::btDefaultCollisionConfiguration>();
    collision_dispatcher_ = std::make_unique<::btCollisionDispatcher>(collision_config_.get());
    broadphase_ = std::make_unique<::btDbvtBroadphase>();
    solver_ = std::make_unique<::btSequentialImpulseConstraintSolver>();
    world_ = std::make_unique<::btDiscreteDynamicsWorld>(
        collision_dispatcher_.get(), broadphase_.get(), solver_.get(), collision_config_.get());
    ghost_pair_callback_ = std::make_unique<::btGhostPairCallback>();
    broadphase_->getOverlappingPairCache()->setInternalGhostPairCallback(ghost_pair_callback_.get());

    world_->setGravity({0.0f, -10.0f, 0.0f});
}

BulletPhysicsSystem::~BulletPhysicsSystem()
{
    try
    {
        for (const auto &body : bodies_)
        {
            remove_body_from_world(body.get(), world_.get());
        }

        for (const auto &controller : character_controllers_)
        {
            remove_body_from_world(controller->rigid_body(), world_.get());
        }
    }
    catch (...)
    {
        LOG_ERROR("physics_system", "exception caught during dtor");
    }
}

void BulletPhysicsSystem::step(std::chrono::milliseconds time_step)
{
    for (auto &controller : character_controllers_)
    {
        controller->update(this, time_step);
    }

    const auto ticks = static_cast<float>(time_step.count());
    world_->stepSimulation(ticks / 1000.0f, 1);

    debug_draw_.update();
}

RigidBody *BulletPhysicsSystem::create_rigid_body(
    const Vector3 &position,
    const CollisionShape *collision_shape,
    RigidBodyType type)
{
    bodies_.emplace_back(
        std::make_unique<BulletRigidBody>(position, static_cast<const BulletCollisionShape *>(collision_shape), type));
    auto *body = static_cast<BulletRigidBody *>(bodies_.back().get());

    if ((body->type() == RigidBodyType::GHOST) || (body->type() == RigidBodyType::CHARACTER_CONTROLLER))
    {
        auto *bullet_ghost = static_cast<btGhostObject *>(body->handle());
        world_->addCollisionObject(bullet_ghost);
    }
    else
    {
        auto *bullet_rigid = static_cast<btRigidBody *>(body->handle());
        world_->addRigidBody(bullet_rigid);
    }

    debug_draw_.register_rigid_body(body);

    return body;
}

const CollisionShape *BulletPhysicsSystem::create_box_collision_shape(const Vector3 &half_size)
{
    const auto &collision_shape = collision_shapes_.emplace_back(std::make_unique<BulletBoxCollisionShape>(half_size));
    debug_draw_.register_box_collision_shape(static_cast<const BulletBoxCollisionShape *>(collision_shape.get()));

    return collision_shape.get();
}

const CollisionShape *BulletPhysicsSystem::create_capsule_collision_shape(float width, float height)
{
    const auto &collision_shape =
        collision_shapes_.emplace_back(std::make_unique<BulletCapsuleCollisionShape>(width, height));
    debug_draw_.register_capsule_collision_shape(
        static_cast<const BulletCapsuleCollisionShape *>(collision_shape.get()));

    return collision_shapes_.back().get();
}

const CollisionShape *BulletPhysicsSystem::create_mesh_collision_shape(const Mesh *mesh, const Vector3 &scale)
{
    const auto &collision_shape =
        collision_shapes_.emplace_back(std::make_unique<BulletMeshCollisionShape>(mesh, scale));
    debug_draw_.register_mesh_collision_shape(static_cast<const BulletMeshCollisionShape *>(collision_shape.get()));

    return collision_shapes_.back().get();
}

const CollisionShape *BulletPhysicsSystem::create_heightmap_collision_shape(
    const Texture *heightmap,
    const Vector3 &scale)
{
    const auto &collision_shape =
        collision_shapes_.emplace_back(std::make_unique<BulletHeightmapCollisionShape>(heightmap, scale));
    debug_draw_.register_height_map_collision_shape(
        static_cast<const BulletHeightmapCollisionShape *>(collision_shape.get()));

    return collision_shapes_.back().get();
}

CharacterController *BulletPhysicsSystem::add(std::unique_ptr<CharacterController> character_controller)
{
    character_controllers_.emplace_back(std::move(character_controller));
    return character_controllers_.back().get();
}

void BulletPhysicsSystem::remove(RigidBody *body)
{
    debug_draw_.deregister_rigid_body(body);
    remove_body_from_world(body, world_.get());

    bodies_.erase(
        std::remove_if(
            std::begin(bodies_), std::end(bodies_), [body](const auto &element) { return element.get() == body; }),
        std::end(bodies_));
}

void BulletPhysicsSystem::remove(CharacterController *character)
{
    remove_body_from_world(character->rigid_body(), world_.get());

    character_controllers_.erase(
        std::remove_if(
            std::begin(character_controllers_),
            std::end(character_controllers_),
            [character](const auto &element) { return element.get() == character; }),
        std::end(character_controllers_));
}

std::vector<RayCastResult> BulletPhysicsSystem::ray_cast(
    const Vector3 &origin,
    const Vector3 &direction,
    const std::set<const RigidBody *> &ignore)
{
    std::vector<RayCastResult> hits;

    // bullet does ray tracing between two vectors, so we create an end vector
    // some great distance away
    btVector3 from{origin.x, origin.y, origin.z};
    const auto far_away = origin + (direction * 10000.0f);
    btVector3 to{far_away.x, far_away.y, far_away.z};

    // create an ignore list of bullet objects
    std::set<const btCollisionObject *> bullet_ignore{};
    std::transform(
        std::cbegin(ignore),
        std::cend(ignore),
        std::inserter(bullet_ignore, std::begin(bullet_ignore)),
        [](const RigidBody *element) {
            const auto *bullet_body = static_cast<const iris::BulletRigidBody *>(element);
            return bullet_body->handle();
        });

    // do the ray cast
    btCollisionWorld::AllHitsRayResultCallback callback{from, to};
    world_->rayTest(from, to, callback);

    if (callback.hasHit())
    {
        // build up collection of hits which aren't being ignored
        for (auto i = 0; i < callback.m_collisionObjects.size(); ++i)
        {
            if (bullet_ignore.count(callback.m_collisionObjects[i]) == 0)
            {
                const auto hit_position = callback.m_hitPointWorld[i];
                const auto *bullet_body = static_cast<const btRigidBody *>(callback.m_collisionObjects[i]);
                auto *body = static_cast<RigidBody *>(bullet_body->getUserPointer());

                hits.push_back(
                    {.body = body, .position = Vector3{hit_position.x(), hit_position.y(), hit_position.z()}});
            }
        }
    }

    // sort the hits from closest to origin
    std::sort(std::begin(hits), std::end(hits), [&origin](const auto &e1, const auto &e2) {
        return Vector3::distance(origin, e1.position) < Vector3::distance(origin, e2.position);
    });

    return hits;
}

std::vector<ContactPoint> BulletPhysicsSystem::contacts(RigidBody *body)
{
    auto *bullet_body = static_cast<iris::BulletRigidBody *>(body);

    CollisionCallback callback{body};
    world_->contactTest(bullet_body->handle(), callback);

    return callback.yield_contact_points();
}

std::unique_ptr<PhysicsState> BulletPhysicsSystem::save()
{
    auto state = std::make_unique<BulletPhysicsState>();

    // save data for all rigid bodies
    for (const auto &body : bodies_)
    {
        auto *bullet_body = static_cast<iris::BulletRigidBody *>(body.get());
        auto *bullet_rigid = static_cast<btRigidBody *>(bullet_body->handle());

        state->bodies.try_emplace(
            bullet_rigid,
            bullet_rigid->getWorldTransform(),
            bullet_rigid->getLinearVelocity(),
            bullet_rigid->getAngularVelocity());
    }

    // save data for all character controllers
    for (const auto &character : character_controllers_)
    {
        auto *bullet_body = static_cast<iris::BulletRigidBody *>(character->rigid_body());
        auto *bullet_rigid = static_cast<btRigidBody *>(bullet_body->handle());

        state->bodies.try_emplace(
            bullet_rigid,
            bullet_rigid->getWorldTransform(),
            bullet_rigid->getLinearVelocity(),
            bullet_rigid->getAngularVelocity());
    }

    return state;
}

void BulletPhysicsSystem::load(const PhysicsState *state)
{
    const auto *bullet_state = static_cast<const BulletPhysicsState *>(state);

    // restore state for each rigid body
    for (const auto &[bullet_body, body_state] : bullet_state->bodies)
    {
        bullet_body->clearForces();

        bullet_body->setWorldTransform(body_state.transform);
        bullet_body->setCenterOfMassTransform(body_state.transform);
        bullet_body->setLinearVelocity(body_state.linear_velocity);
        bullet_body->setAngularVelocity(body_state.angular_velocity);
    }
}

void BulletPhysicsSystem::enable_debug_draw(Scene *scene)
{
    debug_draw_.set_scene(scene);
}

}
