////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_rigid_body.h"

#include <chrono>
#include <memory>
#include <optional>
#include <set>
#include <vector>

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
#include "physics/bullet/bullet_collision_shape.h"
#include "physics/bullet/debug_draw.h"
#include "physics/collision_shape.h"

namespace iris
{

BulletRigidBody::BulletRigidBody(
    const Vector3 &position,
    const BulletCollisionShape *collision_shape,
    RigidBodyType type)
    : name_()
    , type_(type)
    , collision_shape_(collision_shape)
    , body_(nullptr)
    , motion_state_(nullptr)
{
    // convert engine position to bullet form
    btTransform start_transform;
    start_transform.setIdentity();
    start_transform.setOrigin(btVector3(position.x, position.y, position.z));

    auto *shape = collision_shape_->handle();

    // internally we use a derivation of ::btCollisionObject for our rigid
    // body, which one depends on the type:
    //   * GHOST         - ::btGhostObject
    //   * NORMAL/STATIC - ::btRigidBody

    if (type_ == RigidBodyType::GHOST)
    {
        body_ = std::make_unique<btGhostObject>();
        body_->setCollisionShape(shape);
        body_->setWorldTransform(start_transform);
        body_->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }
    else
    {
        // 0 mass means static rigid body
        btScalar mass = (type_ == RigidBodyType::STATIC) ? 0.0f : 10.0f;
        btVector3 localInertia(0, 0, 0);

        if (type_ != RigidBodyType::STATIC)
        {
            shape->calculateLocalInertia(mass, localInertia);
        }

        motion_state_ = std::make_unique<btDefaultMotionState>(start_transform);

        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state_.get(), shape, localInertia);

        body_ = std::make_unique<btRigidBody>(rbInfo);

        body_->setFriction(1.0f);
    }

    // set the user pointer of the bullet object to our engine object, this
    // allows us to get back the RigidBody* when doing collision detection
    body_->setUserPointer(this);
}

Vector3 BulletRigidBody::position() const
{
    const auto transform = body_->getWorldTransform();

    return {transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z()};
}

Quaternion BulletRigidBody::orientation() const
{
    const auto transform = body_->getWorldTransform();
    const auto orientation = transform.getRotation();

    return {orientation.x(), orientation.y(), orientation.z(), orientation.w()};
}

Vector3 BulletRigidBody::linear_velocity() const
{
    Vector3 velocity{};

    if (type_ != RigidBodyType::GHOST)
    {
        const auto bullet_velocity = static_cast<btRigidBody *>(body_.get())->getLinearVelocity();

        velocity = {bullet_velocity.x(), bullet_velocity.y(), bullet_velocity.z()};
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling linear_velocity on ghost object");
    }

    return velocity;
}

Vector3 BulletRigidBody::angular_velocity() const
{
    Vector3 velocity{};

    if (type_ != RigidBodyType::GHOST)
    {
        const auto bullet_velocity = static_cast<btRigidBody *>(body_.get())->getAngularVelocity();

        velocity = {bullet_velocity.x(), bullet_velocity.y(), bullet_velocity.z()};
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling angular_velocity on ghost object");
    }

    return velocity;
}

void BulletRigidBody::set_linear_velocity(const Vector3 &linear_velocity)
{
    if (type_ != RigidBodyType::GHOST)
    {
        btVector3 velocity{linear_velocity.x, linear_velocity.y, linear_velocity.z};

        static_cast<btRigidBody *>(body_.get())->setLinearVelocity(velocity);
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling set_linear_velocity on ghost object");
    }
}

void BulletRigidBody::set_angular_velocity(const Vector3 &angular_velocity)
{
    if (type_ != RigidBodyType::GHOST)
    {
        btVector3 velocity{angular_velocity.x, angular_velocity.y, angular_velocity.z};

        static_cast<btRigidBody *>(body_.get())->setAngularVelocity(velocity);
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling set_angular_velocity on ghost object");
    }
}

void BulletRigidBody::reposition(const Vector3 &position, const Quaternion &orientation)
{
    btTransform transform;
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    transform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));

    body_->setWorldTransform(transform);

    // also update the motion state for non ghost rigid bodies
    if (motion_state_ != nullptr)
    {
        motion_state_->setWorldTransform(transform);
    }
}

std::string BulletRigidBody::name() const
{
    return name_;
}

void BulletRigidBody::set_name(const std::string &name)
{
    name_ = name;
}

RigidBodyType BulletRigidBody::type() const
{
    return type_;
}

const CollisionShape *BulletRigidBody::collision_shape() const
{
    return collision_shape_;
}

void BulletRigidBody::set_collision_shape(const CollisionShape *collision_shape)
{
    collision_shape_ = static_cast<const BulletCollisionShape *>(collision_shape);

    body_->setCollisionShape(collision_shape_->handle());
}

void BulletRigidBody::apply_impulse(const Vector3 &impulse)
{
    if (type_ != RigidBodyType::GHOST)
    {
        static_cast<btRigidBody *>(body_.get())
            ->applyImpulse(btVector3{impulse.x, impulse.y, impulse.z}, ::btVector3{});
    }
    else
    {
        LOG_ENGINE_WARN("physics", "calling apply_impulse on ghost object");
    }
}

btCollisionObject *BulletRigidBody::handle() const
{
    return body_.get();
}

}
