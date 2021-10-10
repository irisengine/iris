////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_box_collision_shape.h"

#include <memory>

#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include "core/vector3.h"

namespace iris
{

BulletBoxCollisionShape::BulletBoxCollisionShape(const Vector3 &half_size)
    : shape_(std::make_unique<btBoxShape>(btVector3{half_size.x, half_size.y, half_size.z}))
{
}

btCollisionShape *BulletBoxCollisionShape::handle() const
{
    return shape_.get();
}

}
