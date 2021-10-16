////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include "physics/collision_shape.h"

namespace iris
{

/**
 * Implementation of CollisionShape for bullet.
 */
class BulletCollisionShape : public CollisionShape
{
  public:
    ~BulletCollisionShape() override = default;

    /**
     * Get a handle to the bullet object.
     *
     * @returns
     *   Bullet object.
     */
    virtual btCollisionShape *handle() const = 0;
};

}
