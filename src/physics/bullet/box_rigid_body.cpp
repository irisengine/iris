#include "physics/box_rigid_body.h"

#include <memory>

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <LinearMath/btDefaultMotionState.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace iris
{

struct BoxRigidBody::implementation
{
    std::unique_ptr<::btRigidBody> body;
    std::unique_ptr<::btBoxShape> shape;
    std::unique_ptr<::btDefaultMotionState> motion_state;
};

BoxRigidBody::BoxRigidBody(
    const Vector3 &position,
    const Vector3 &half_size,
    bool is_static)
    : impl_(std::make_unique<implementation>())
{
    // 0 mass means static rigid body
    ::btScalar mass = is_static ? 0.0f : 10.0f;

    impl_->shape = std::make_unique<::btBoxShape>(
        ::btVector3(half_size.x, half_size.y, half_size.z));

    ::btVector3 localInertia(0, 0, 0);
    impl_->shape->calculateLocalInertia(mass, localInertia);

    ::btTransform start_transform;
    start_transform.setIdentity();
    start_transform.setOrigin(::btVector3(position.x, position.y, position.z));
    impl_->motion_state = std::make_unique<::btDefaultMotionState>(start_transform);

    ::btRigidBody::btRigidBodyConstructionInfo rbInfo(
        mass,
        impl_->motion_state.get(),
        impl_->shape.get(),
        localInertia);

    impl_->body = std::make_unique<::btRigidBody>(rbInfo);

    impl_->body->setFriction(1.0f);
}

BoxRigidBody::~BoxRigidBody() = default;

Vector3 BoxRigidBody::position() const
{
	::btTransform transform;
    impl_->body->getMotionState()->getWorldTransform(transform);

    return{ transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z() };
}

Quaternion BoxRigidBody::orientation() const
{
    const auto orientation = impl_->body->getOrientation();
    const auto axis = orientation.getAxis();

    return { Vector3{ axis.x(), axis.y(), axis.z() }, orientation.getAngle() };
}

Vector3 BoxRigidBody::linear_velocity() const
{
    const auto velocity = impl_->body->getLinearVelocity();

    return { velocity.x(), velocity.y(), velocity.z() };
}

Vector3 BoxRigidBody::angular_velocity() const
{
    const auto velocity = impl_->body->getAngularVelocity();

    return { velocity.x(), velocity.y(), velocity.z() };
}

void BoxRigidBody::set_linear_velocity(const Vector3 &linear_velocity)
{
    ::btVector3 velocity{ linear_velocity.x, linear_velocity.y, linear_velocity. z };
    impl_->body->setLinearVelocity(velocity);
}

void BoxRigidBody::set_angular_velocity(const Vector3 &angular_velocity)
{
    ::btVector3 velocity{ angular_velocity.x, angular_velocity.y, angular_velocity. z };
    impl_->body->setAngularVelocity(velocity);
}

void BoxRigidBody::reposition(const Vector3 &position, const Quaternion &orientation)
{
    ::btTransform transform;
    transform.setOrigin(::btVector3(position.x, position.y, position.z));
    transform.setRotation(::btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));

    impl_->body->setWorldTransform(transform);
    impl_->motion_state->setWorldTransform(transform);
}

std::any BoxRigidBody::native_handle() const
{
    return impl_->body.get();
}

}
