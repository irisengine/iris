////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
     *   The extents from the centre of the box which define its size.
     */
    explicit BulletBoxCollisionShape(const Vector3 &half_size);

    ~BulletBoxCollisionShape() override = default;

    /**
     * Get box half size.
     *
     * @returns
     *   The extents from the centre of the box which define its size.
     */
    Vector3 half_size() const;

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

    /** The extents from the centre of the box which define its size. */
    Vector3 half_size_;
};

}
