#include "physics/basic_character_controller.h"

#include <btBulletDynamicsCommon.h>

#include "core/exception.h"
#include "core/vector3.h"
#include "log/log.h"
#include "physics/bullet/bullet_rigid_body.h"
#include "physics/collision_shape.h"
#include "physics/physics_system.h"
#include "physics/rigid_body_type.h"

namespace iris
{

BasicCharacterController::BasicCharacterController(
    PhysicsSystem *physics_system)
    : speed_(12.0f)
    , mass_(62.0f)
    , physics_system_(physics_system)
    , body_(nullptr)
{
    // use capsule shape for character
    body_ = physics_system_->create_rigid_body(
        Vector3{0.0f, 0.0f, 10.0f},
        physics_system_->create_capsule_collision_shape(0.5f, 1.7f),
        RigidBodyType::NORMAL);

    auto *bullet_body = static_cast<BulletRigidBody *>(body_);

    if (bullet_body->type() != RigidBodyType::NORMAL)
    {
        throw Exception(
            "can only create BasicCharacterController with a NORMAL RigidBody");
    }

    auto *rigid_body = static_cast<btRigidBody *>(bullet_body->handle());

    // prevent capsule from falling over
    rigid_body->setAngularFactor(::btVector3(0.0f, 0.0f, 0.0f));

    // prevent bullet sleeping the rigid body
    rigid_body->setActivationState(DISABLE_DEACTIVATION);
}

BasicCharacterController::~BasicCharacterController() = default;

void BasicCharacterController::set_walk_direction(const Vector3 &direction)
{
    const auto current_velocity = body_->linear_velocity();
    const auto velocity = direction * speed_;

    body_->set_linear_velocity({velocity.x, current_velocity.y, velocity.z});
}

Vector3 BasicCharacterController::position() const
{
    return body_->position();
}

Quaternion BasicCharacterController::orientation() const
{
    return body_->orientation();
}

Vector3 BasicCharacterController::linear_velocity() const
{
    return body_->linear_velocity();
}

Vector3 BasicCharacterController::angular_velocity() const
{
    return body_->angular_velocity();
}

void BasicCharacterController::set_linear_velocity(
    const Vector3 &linear_velocity)
{
    body_->set_linear_velocity(linear_velocity);
}

void BasicCharacterController::set_angular_velocity(
    const Vector3 &angular_velocity)
{
    body_->set_angular_velocity(angular_velocity);
}

void BasicCharacterController::set_speed(float speed)
{
    speed_ = speed;
}

void BasicCharacterController::reposition(
    const Vector3 &position,
    const Quaternion &orientation)
{
    body_->reposition(position, orientation);
}

void BasicCharacterController::jump()
{
    // if we are on the ground then jump by applying an upwards impulse
    if (on_ground())
    {
        body_->apply_impulse({0.0f, mass_ / 10.0f, 0.0f});
    }
}

bool BasicCharacterController::on_ground() const
{
    auto ground = false;

    // cast a ray downwards to see if what is below us
    const auto hit = physics_system_->ray_cast(position(), {0.0f, -1.0f, 0.0f});

    if (hit)
    {
        // we are on the ground if the closest object is less than our height
        ground = (std::get<1>(*hit) - position()).magnitude() < 1.7f;
    }

    return ground;
}

RigidBody *BasicCharacterController::rigid_body() const
{
    return body_;
}

void BasicCharacterController::set_collision_shape(
    CollisionShape *collision_shape)
{
    body_->set_collision_shape(collision_shape);
}

}
