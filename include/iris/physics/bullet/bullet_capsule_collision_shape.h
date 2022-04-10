////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

#include "physics/bullet/bullet_collision_shape.h"

namespace iris
{

/**
 * Implementation of CollisionShape for a capsule.
 */
class BulletCapsuleCollisionShape : public BulletCollisionShape
{
  public:
    /**
     * Construct new CapsuleCollisionShape
     *
     * @param width
     *   Diameter of capsule.
     *
     * @param height
     *   Height of capsule.
     */
    BulletCapsuleCollisionShape(float width, float height);

    ~BulletCapsuleCollisionShape() override = default;

    /**
     * Get width of capsule.
     *
     * @returns
     *   Capsule diameter.
     */
    float width() const;

    /**
     * Get height of capsule.
     *
     * @returns
     *   Capsule height.
     */
    float height() const;

    /**
     * Get a handle to the bullet object.
     *
     * @returns
     *   Bullet object.
     */
    btCollisionShape *handle() const override;

  private:
    /** Bullet collision shape. */
    std::unique_ptr<btCapsuleShape> shape_;

    /** Diameter of capsule. */
    float width_;

    /** Height of capsule. */
    float height_;
};

}
