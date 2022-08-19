////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

#include "core/vector3.h"
#include "physics/contact_point.h"
#include "physics/rigid_body.h"

namespace iris
{

/**
 * Implementation of bullet ContactResultCallback, which is used for testing collisions with a rigid body.
 */
class CollisionCallback : public btCollisionWorld::ContactResultCallback
{
  public:
    /**
     * Construct a new CollisionCallback.
     *
     * @param testing_body
     *   The body being tested for collisions, this will always be "contact_a" in ContactPoint.
     */
    CollisionCallback(RigidBody *testing_body);

    /**
     * Callback to register a collision with the testing body.
     *
     * @param cp
     *   Information of contact point.
     *
     * @param colObj0Wrap
     *   First colliding object.
     *
     * @param colObj1Wrap
     *   Second colliding object.
     *
     * @returns
     *   Unused by bullet, we always return 0.
     */
    btScalar addSingleResult(
        btManifoldPoint &cp,
        const btCollisionObjectWrapper *colObj0Wrap,
        int,
        int,
        const btCollisionObjectWrapper *colObj1Wrap,
        int,
        int) override;

    /**
     * Yields all registered ContactPoint objects.
     *
     * @returns
     *   All registered contacts.
     */
    std::vector<ContactPoint> yield_contact_points();

  private:
    /** The rigid body being tested for collisions. */
    RigidBody *testing_body_;

    /** Collection of registered contacts. */
    std::vector<ContactPoint> contact_points_;
};

}
