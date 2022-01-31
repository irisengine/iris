////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/collision_callback.h"

#include <vector>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

#include "core/vector3.h"
#include "physics/contact_point.h"
#include "physics/rigid_body.h"

namespace
{

/**
 * Helper function to get the engine rigid body object from the bullet object provided in the callback.
 *
 * @param bullet_collision_wrapper
 *   Bullet object provided in collision callback.
 *
 * @returns
 *   Engine rigid body object.
 */
iris::RigidBody *get_engine_body(const btCollisionObjectWrapper *bullet_collision_wrapper)
{
    auto *user_ptr = bullet_collision_wrapper->getCollisionObject()->getUserPointer();

    return user_ptr != nullptr ? static_cast<iris::RigidBody *>(user_ptr) : nullptr;
}

}

namespace iris
{

CollisionCallback::CollisionCallback(RigidBody *testing_body)
    : testing_body_(testing_body)
    , contact_points_()
{
}

btScalar CollisionCallback::addSingleResult(
    btManifoldPoint &cp,
    const btCollisionObjectWrapper *colObj0Wrap,
    int,
    int,
    const btCollisionObjectWrapper *colObj1Wrap,
    int,
    int)
{
    // get the engine rigid body objects from the bullet objects
    RigidBody *body_a = get_engine_body(colObj0Wrap);
    RigidBody *body_b = get_engine_body(colObj1Wrap);

    // get penetration - the way bullet does its collision detection it may return contacts that are not actually
    // penetrating however we can use the distance to differentiate false positives
    // a negative depth means the object is penetrating and positive depth means it is not
    const auto penetration = cp.getDistance();

    if ((penetration < 0.0f) && (body_a != nullptr) && (body_b != nullptr))
    {
        // get world position and normal of collision contact point
        const auto bullet_world_pos = cp.getPositionWorldOnA();
        const auto normal = cp.m_normalWorldOnB;

        contact_points_.push_back(
            {.contact = (body_a == testing_body_) ? body_b : body_a,
             .position = {bullet_world_pos.x(), bullet_world_pos.y(), bullet_world_pos.z()},
             .penetration = -penetration, // normalise penetration value to be positive
             .normal = {normal.x(), normal.y(), normal.z()}});
    }

    // bullet ignores the return value of this function
    return 0;
}

std::vector<ContactPoint> CollisionCallback::yield_contact_points()
{
    std::vector<ContactPoint> contacts{};
    std::swap(contacts, contact_points_);

    // sort from most to least penetrated
    std::sort(
        std::begin(contacts),
        std::end(contacts),
        [](const ContactPoint &cp1, const ContactPoint &cp2) { return cp1.penetration > cp2.penetration; });

    return contacts;
}

}
