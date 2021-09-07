#pragma once

#include <memory>

#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include "core/vector3.h"
#include "physics/bullet/bullet_collision_shape.h"

namespace iris
{

/**
 * Implementation of CollisionShape for a box.
 */
class BulletBoxCollisionShape : public BulletCollisionShape
{
  public:
    /**
     * Construct a new BoxCollisionShape
     *
     * @param half_size
     *   The extends from the center of the box which define its size.
     */
    explicit BulletBoxCollisionShape(const Vector3 &half_size);

    ~BulletBoxCollisionShape() override = default;

    /**
     * Get a handle to the bullet object.
     *
     * @returns
     *   Bullet object.
     */
    btCollisionShape *handle() const override;

  private:
    /** Bullet collision shape. */
    std::unique_ptr<btBoxShape> shape_;
};

}
