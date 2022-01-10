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

    if ((body_a != nullptr) && (body_b != nullptr))
    {
        // get world position of collision contact point
        const auto bullet_world_pos = cp.getPositionWorldOnA();

        ContactPoint contact{
            .contact_a = body_a,
            .contact_b = body_b,
            .position = {bullet_world_pos.x(), bullet_world_pos.y(), bullet_world_pos.z()},
        };

        // we always want to return the testing object as body_a
        // as we cannot guarantee the ordering of objects from bullet we check here and swap if necessary

        if (body_a != testing_body_)
        {
            std::swap(contact.contact_a, contact.contact_b);
        }

        // sanity check that we definitely have a contact with our testing body
        if (body_a == testing_body_)
        {
            contact_points_.emplace_back(contact);
        }
    }

    // bullet ignores the return value of this function
    return 0;
}

std::vector<ContactPoint> CollisionCallback::yield_contact_points()
{
    std::vector<ContactPoint> contacts{};
    std::swap(contacts, contact_points_);

    return contacts;
}

}
