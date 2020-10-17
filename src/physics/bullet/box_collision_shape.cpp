#include "physics/box_collision_shape.h"

#include <any>
#include <memory>

#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include "core/vector3.h"

namespace iris
{

struct BoxCollisionShape::implementation
{
    ::btBoxShape shape = ::btBoxShape{::btVector3{}};
};

BoxCollisionShape::BoxCollisionShape(const Vector3 &half_size)
    : impl_(std::make_unique<implementation>())
{
    impl_->shape =
        ::btBoxShape{::btVector3{half_size.x, half_size.y, half_size.z}};
}

BoxCollisionShape::~BoxCollisionShape() = default;

std::any BoxCollisionShape::native_handle() const
{
    return {static_cast<::btCollisionShape *>(std::addressof(impl_->shape))};
}

}
