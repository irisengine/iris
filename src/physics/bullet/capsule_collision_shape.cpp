#include "physics/capsule_collision_shape.h"

#include <any>
#include <memory>

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

namespace iris
{

struct CapsuleCollisionShape::implementation
{
    ::btCapsuleShape shape = ::btCapsuleShape{0.0f, 0.0f};
};

CapsuleCollisionShape::CapsuleCollisionShape(float width, float height)
    : impl_(std::make_unique<implementation>())
{
    impl_->shape = ::btCapsuleShape{width, height};
}

CapsuleCollisionShape::~CapsuleCollisionShape() = default;

std::any CapsuleCollisionShape::native_handle() const
{
    return {static_cast<::btCollisionShape *>(std::addressof(impl_->shape))};
}

}
