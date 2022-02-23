////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_heightmap_collision_shape.h"

#include <memory>
#include <vector>

#include <BulletCollision/CollisionShapes/btHeightFieldTerrainShape.h>

#include "core/error_handling.h"
#include "graphics/texture.h"

namespace iris
{

BulletHeightmapCollisionShape::BulletHeightmapCollisionShape(const Texture *heightmap, const Vector3 &scale)
    : shape_(nullptr)
    , height_data_()
{
    // extract r component from heightmap and store in a local copy, this is needed as bullet only takes a pointer
    // to the data and doesn't make a copy
    for (auto iter = std::cbegin(heightmap->data()); iter != std::cend(heightmap->data()); iter += 4u)
    {
        height_data_.emplace_back((static_cast<float>(*iter) / 255.0f) * scale.y);
    }

    shape_ = std::make_unique<btHeightfieldTerrainShape>(
        static_cast<int>(heightmap->width()),
        static_cast<int>(heightmap->height()),
        height_data_.data(),
        1.0f,
        0.0f,
        scale.y,
        1,
        PHY_FLOAT,
        false);

    shape_->setLocalScaling(btVector3{scale.x, scale.y, scale.z});
}

btCollisionShape *BulletHeightmapCollisionShape::handle() const
{
    return shape_.get();
}

}
