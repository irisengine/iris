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
