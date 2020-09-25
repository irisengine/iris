#include "physics/basic_character_controller.h"

#include <btBulletDynamicsCommon.h>

#include "core/root.h"
#include "core/vector3.h"
#include "log/log.h"
#include "physics/capsule_rigid_body.h"
#include "physics/physics_system.h"

namespace iris
{

struct BasicCharacterController::implementation
{
    std::unique_ptr<CapsuleRigidBody> body;
    ::btRigidBody *bullet_rigid_body;
};

BasicCharacterController::BasicCharacterController()
    : speed_(12.0f)
    , mass_(62.0f)
    , impl_(std::make_unique<implementation>())
{
    // use capsule shape for character
    impl_->body = std::make_unique<CapsuleRigidBody>(
        Vector3{0.0f, 0.0f, 10.0f}, 0.5f, 1.7f, false);

    // store a copy of the bullet rigid body pointer
    impl_->bullet_rigid_body =
        std::any_cast<::btRigidBody *>(impl_->body->native_handle());

    // prevent capsule from falling over
    impl_->bullet_rigid_body->setAngularFactor(::btVector3(0.0f, 0.0f, 0.0f));

    // prevent bullet sleeping the rigid body
    impl_->bullet_rigid_body->setActivationState(DISABLE_DEACTIVATION);
}

BasicCharacterController::~BasicCharacterController() = default;

void BasicCharacterController::set_walk_direction(const Vector3 &direction)
{
    const auto current_velocity = impl_->bullet_rigid_body->getLinearVelocity();
    const auto velocity = direction * speed_;

    impl_->bullet_rigid_body->setLinearVelocity(
        ::btVector3{velocity.x, current_velocity.y(), velocity.z});
}

Vector3 BasicCharacterController::position() const
{
    return impl_->body->position();
}

Vector3 BasicCharacterController::linear_velocity() const
{
    return impl_->body->linear_velocity();
}

Vector3 BasicCharacterController::angular_velocity() const
{
    return impl_->body->angular_velocity();
}

void BasicCharacterController::set_linear_velocity(
    const Vector3 &linear_velocity)
{
    impl_->body->set_linear_velocity(linear_velocity);
}

void BasicCharacterController::set_angular_velocity(
    const Vector3 &angular_velocity)
{
    impl_->body->set_angular_velocity(angular_velocity);
}

void BasicCharacterController::set_speed(real speed)
{
    speed_ = speed;
}

void BasicCharacterController::reposition(
    const Vector3 &position,
    const Quaternion &orientation)
{
    impl_->body->reposition(position, orientation);
}

void BasicCharacterController::jump()
{
    // if we are on the ground then jump by applying an upwards impulse
    if (on_ground())
    {
        impl_->bullet_rigid_body->applyImpulse(
            ::btVector3{0.0f, mass_, 0.0f}, ::btVector3{});
    }
}

bool BasicCharacterController::on_ground() const
{
    auto ground = false;

    // cast a ray downwards to see if what is below us
    const auto hit =
        Root::physics_system().ray_cast(position(), {0.0f, -1.0f, 0.0f});

    if (hit)
    {
        // we are on the ground if the closest object is less than our height
        ground = (*hit - position()).magnitude() < 1.7f;
    }

    return ground;
}

std::any BasicCharacterController::native_handle() const
{
    return impl_->bullet_rigid_body;
}

}
