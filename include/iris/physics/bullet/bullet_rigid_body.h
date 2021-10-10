////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <LinearMath/btDefaultMotionState.h>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/bullet/bullet_collision_shape.h"
#include "physics/rigid_body.h"
#include "physics/rigid_body_type.h"

namespace iris
{

/**
 * Implementation of RigidBody for bullet.
 */
class BulletRigidBody : public RigidBody
{
  public:
    /**
     * Construct a new BulletRigidBody.
     *
     * @param position
     *   Position in world space.
     *
     * @param collision_shape
     *   The shape that defined the rigid body, this is used for collision
     *   detection/response.
     *
     * @param type
     *   The type of rigid body, this effects how this body interacts with
     *   others.
     */
    BulletRigidBody(const Vector3 &position, BulletCollisionShape *collision_shape, RigidBodyType type);

    ~BulletRigidBody() override = default;

    /**
     * Get position of rigid body centre of mass.
     *
     * @returns
     *   Rigid body position.
     */
    Vector3 position() const override;

    /**
     * Get orientation of rigid body.
     *
     * @returns
     *   Rigid body orientation.
     */
    Quaternion orientation() const override;

    /**
     * Get linear velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @returns
     *   Linear velocity.
     */
    Vector3 linear_velocity() const override;

    /**
     * Get angular velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @returns
     *   Angular velocity.
     */
    Vector3 angular_velocity() const override;

    /**
     * Set linear velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @param linear_velocity
     *   New linear velocity.
     */
    void set_linear_velocity(const Vector3 &linear_velocity) override;

    /**
     * Set angular velocity.
     *
     * This is only valid for non GHOST type rigid bodies.
     *
     * @param angular_velocity
     *   New angular velocity.
     */
    void set_angular_velocity(const Vector3 &angular_velocity) override;

    /**
     * Reposition rigid body.
     *
     * @param position
     *   New position.
     *
     * @param orientation
     *   New orientation.
     */
    void reposition(const Vector3 &position, const Quaternion &orientation) override;

    /**
     * Get the name of the rigid body. This is an optional trait and will return
     * an empty string if a name has not already been set.
     *
     * @returns
     *   Optional name of rigid body.
     */
    std::string name() const override;

    /**
     * Set name.
     *
     * @param name
     *   New name.
     */
    void set_name(const std::string &name) override;

    /**
     * Get type.
     *
     * @returns
     *   Type of rigid body.
     */
    RigidBodyType type() const override;

    /**
     * Pointer to collision shape.
     *
     * @returns
     *   Collision shape.
     */
    CollisionShape *collision_shape() const override;

    /**
     * Set collision shape.
     *
     * @param collision_shape
     *   New collision shape.
     */
    void set_collision_shape(CollisionShape *collision_shape) override;

    /**
     * Apply an impulse (at the centre of mass).
     *
     * @param impulse
     *   Impulse to apply.
     */
    void apply_impulse(const Vector3 &impulse) override;

    /**
     * Get a handle to the bullet object.
     *
     * @returns
     *   Bullet object.
     */
    btCollisionObject *handle() const;

  private:
    /** Name of rigid body.*/
    std::string name_;

    /** Type of rigid body. */
    RigidBodyType type_;

    /** Collision shape of rigid body. */
    BulletCollisionShape *collision_shape_;

    /** Bullet collision object. */
    std::unique_ptr<btCollisionObject> body_;

    /** Bullet motion state. */
    std::unique_ptr<btDefaultMotionState> motion_state_;
};

}
