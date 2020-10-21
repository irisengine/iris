#include "physics/rigid_body.h"

#include <any>
#include <memory>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <LinearMath/btDefaultMotionState.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "core/exception.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "log/log.h"
#include "physics/collision_shape.h"

namespace iris
{

struct RigidBody::implementation
{
    std::unique_ptr<::btCollisionObject> body;
    std::unique_ptr<::btDefaultMotionState> motion_state;
};

RigidBody::RigidBody(
    const Vector3 &position,
    std::unique_ptr<CollisionShape> collision_shape,
    RigidBodyType type)
    : name_()
    , type_(type)
    , collision_shape_(std::move(collision_shape))
    , impl_(std::make_unique<implementation>())
{
    // convert engine position to bullet form
    ::btTransform start_transform;
    start_transform.setIdentity();
    start_transform.setOrigin(::btVector3(position.x, position.y, position.z));

    auto *shape =
        std::any_cast<::btCollisionShape *>(collision_shape_->native_handle());

    // internally we use a derivation of ::btCollisionObject for our rigid
    // body, which one depends on the type:
    //   * GHOST         - ::btGhostObject
    //   * NORMAL/STATIC - ::btRigidBody

    if (type_ == RigidBodyType::GHOST)
    {
        impl_->body = std::make_unique<::btGhostObject>();
        impl_->body->setCollisionShape(shape);
        impl_->body->setWorldTransform(start_transform);
        impl_->body->setCollisionFlags(
            ::btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }
    else
    {
        // 0 mass means static rigid body
        ::btScalar mass = (type_ == RigidBodyType::STATIC) ? 0.0f : 10.0f;

        ::btVector3 localInertia(0, 0, 0);
        shape->calculateLocalInertia(mass, localInertia);

        impl_->motion_state =
            std::make_unique<::btDefaultMotionState>(start_transform);

        ::btRigidBody::btRigidBodyConstructionInfo rbInfo(
            mass, impl_->motion_state.get(), shape, localInertia);

        impl_->body = std::make_unique<::btRigidBody>(rbInfo);

        impl_->body->setFriction(1.0f);
    }

    // set the user pointer of the bullet object to our engine object, this
    // allows us to get back the RigidBody* when doing collision detection
    impl_->body->setUserPointer(this);
}

RigidBody::~RigidBody() = default;

Vector3 RigidBody::position() const
{
    const auto transform = impl_->body->getWorldTransform();

    return {
        transform.getOrigin().x(),
        transform.getOrigin().y(),
        transform.getOrigin().z()};
}

Quaternion RigidBody::orientation() const
{
    const auto transform = impl_->body->getWorldTransform();
    const auto orientation = transform.getRotation();

    return {orientation.x(), orientation.y(), orientation.z(), orientation.w()};
}

Vector3 RigidBody::linear_velocity() const
{
    Vector3 velocity{};

    if (type_ != RigidBodyType::GHOST)
    {
        const auto bullet_velocity =
            static_cast<::btRigidBody *>(impl_->body.get())
                ->getLinearVelocity();

        velocity = {
            bullet_velocity.x(), bullet_velocity.y(), bullet_velocity.z()};
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling linear_velocity on ghost object");
    }

    return velocity;
}

Vector3 RigidBody::angular_velocity() const
{
    Vector3 velocity{};

    if (type_ != RigidBodyType::GHOST)
    {
        const auto bullet_velocity =
            static_cast<::btRigidBody *>(impl_->body.get())
                ->getAngularVelocity();

        velocity = {
            bullet_velocity.x(), bullet_velocity.y(), bullet_velocity.z()};
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling angular_velocity on ghost object");
    }

    return velocity;
}

void RigidBody::set_linear_velocity(const Vector3 &linear_velocity)
{
    if (type_ != RigidBodyType::GHOST)
    {
        ::btVector3 velocity{
            linear_velocity.x, linear_velocity.y, linear_velocity.z};

        static_cast<::btRigidBody *>(impl_->body.get())
            ->setLinearVelocity(velocity);
    }
    else
    {
        LOG_ENGINE_WARN(
            "physics", "calling set_linear_velocity on ghost object");
    }
}

void RigidBody::set_angular_velocity(const Vector3 &angular_velocity)
{
    if (type_ != RigidBodyType::GHOST)
    {
        ::btVector3 velocity{
            angular_velocity.x, angular_velocity.y, angular_velocity.z};

        static_cast<::btRigidBody *>(impl_->body.get())
            ->setAngularVelocity(velocity);
    }
    else
    {
        LOG_ENGINE_WARN(
            "physics", "calling set_angular_velocity on ghost object");
    }
}

void RigidBody::reposition(
    const Vector3 &position,
    const Quaternion &orientation)
{
    ::btTransform transform;
    transform.setOrigin(::btVector3(position.x, position.y, position.z));
    transform.setRotation(::btQuaternion(
        orientation.x, orientation.y, orientation.z, orientation.w));

    impl_->body->setWorldTransform(transform);

    // also update the motion state for non ghost rigid bodies
    if (impl_->motion_state != nullptr)
    {
        impl_->motion_state->setWorldTransform(transform);
    }
}

std::any RigidBody::native_handle() const
{
    // we return the derived type, it is assumed that someone calling
    // native_handle knows they type they are expecting
    return (type_ == RigidBodyType::GHOST)
               ? std::any{static_cast<::btGhostObject *>(impl_->body.get())}
               : std::any{static_cast<::btRigidBody *>(impl_->body.get())};
}

std::string RigidBody::name() const
{
    return name_;
}

void RigidBody::set_name(const std::string &name)
{
    name_ = name;
}

RigidBodyType RigidBody::type() const
{
    return type_;
}

CollisionShape *RigidBody::collision_shape() const
{
    return collision_shape_.get();
}

}
