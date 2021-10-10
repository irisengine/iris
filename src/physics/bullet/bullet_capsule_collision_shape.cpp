////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_capsule_collision_shape.h"

#include <memory>

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

namespace iris
{

BulletCapsuleCollisionShape::BulletCapsuleCollisionShape(
    float width,
    float height)
    : shape_(std::make_unique<btCapsuleShape>(width, height))
{
}

btCollisionShape *BulletCapsuleCollisionShape::handle() const
{
    return shape_.get();
}

}
