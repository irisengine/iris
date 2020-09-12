#include "physics/capsule_rigid_body.h"

#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <memory>

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace iris
{

struct CapsuleRigidBody::implementation
{
    std::unique_ptr<::btRigidBody> body;
    std::unique_ptr<::btCapsuleShape> shape;
    std::unique_ptr<::btDefaultMotionState> motion_state;
};

CapsuleRigidBody::CapsuleRigidBody(
    const Vector3 &position,
    real width,
    real height,
    bool is_static)
    : impl_(std::make_unique<implementation>())
{
    // 0 mass means static rigid body
    ::btScalar mass = is_static ? 0.0f : 62.0f;

    impl_->shape = std::make_unique<::btCapsuleShape>(width, height);

    ::btVector3 localInertia(0, 0, 0);
    impl_->shape->calculateLocalInertia(mass, localInertia);

    ::btTransform start_transform;
    start_transform.setIdentity();
    start_transform.setOrigin(::btVector3(position.x, position.y, position.z));
    impl_->motion_state =
        std::make_unique<::btDefaultMotionState>(start_transform);

    ::btRigidBody::btRigidBodyConstructionInfo rbInfo(
        mass, impl_->motion_state.get(), impl_->shape.get(), localInertia);

    impl_->body = std::make_unique<::btRigidBody>(rbInfo);

    impl_->body->setFriction(1.0f);
}

CapsuleRigidBody::~CapsuleRigidBody() = default;

Vector3 CapsuleRigidBody::position() const
{
    ::btTransform transform;
    impl_->body->getMotionState()->getWorldTransform(transform);

    return {
        transform.getOrigin().x(),
        transform.getOrigin().y(),
        transform.getOrigin().z()};
}

Quaternion CapsuleRigidBody::orientation() const
{
    const auto orientation = impl_->body->getOrientation();
    const auto axis = orientation.getAxis();

    return {Vector3{axis.x(), axis.y(), axis.z()}, orientation.getAngle()};
}

Vector3 CapsuleRigidBody::linear_velocity() const
{
    const auto velocity = impl_->body->getLinearVelocity();

    return {velocity.x(), velocity.y(), velocity.z()};
}

Vector3 CapsuleRigidBody::angular_velocity() const
{
    const auto velocity = impl_->body->getAngularVelocity();

    return {velocity.x(), velocity.y(), velocity.z()};
}

void CapsuleRigidBody::set_linear_velocity(const Vector3 &linear_velocity)
{
    ::btVector3 velocity{
        linear_velocity.x, linear_velocity.y, linear_velocity.z};
    impl_->body->setLinearVelocity(velocity);
}

void CapsuleRigidBody::set_angular_velocity(const Vector3 &angular_velocity)
{
    ::btVector3 velocity{
        angular_velocity.x, angular_velocity.y, angular_velocity.z};
    impl_->body->setAngularVelocity(velocity);
}

void CapsuleRigidBody::reposition(
    const Vector3 &position,
    const Quaternion &orientation)
{
    ::btTransform transform;
    transform.setOrigin(::btVector3(position.x, position.y, position.z));
    transform.setRotation(::btQuaternion(
        orientation.x, orientation.y, orientation.z, orientation.w));

    impl_->body->setWorldTransform(transform);
    impl_->motion_state->setWorldTransform(transform);
}

std::any CapsuleRigidBody::native_handle() const
{
    return impl_->body.get();
}

}
