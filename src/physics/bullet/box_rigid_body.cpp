#include "physics/box_rigid_body.h"

#include <memory>

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <LinearMath/btDefaultMotionState.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace eng
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

std::any BoxRigidBody::native_handle() const
{
    return impl_->body.get();
}

}

