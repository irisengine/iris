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
