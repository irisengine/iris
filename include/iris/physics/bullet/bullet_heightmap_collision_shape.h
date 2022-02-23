////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <vector>

#include <BulletCollision/CollisionShapes/btHeightFieldTerrainShape.h>

#include "core/vector3.h"
#include "physics/bullet/bullet_collision_shape.h"

namespace iris
{

class Texture;

/**
 * Implementation of CollisionShape for a texture (reads height data from r component).
 */
class BulletHeightmapCollisionShape : public BulletCollisionShape
{
  public:
    /**
     * Construct a new BulletHeightMaptCollisionShape
     *
     * @param texture
     *   Texture containing height data.
     *
     * @param scale
     *   The scale of the model as it will be rendered.
     */
    explicit BulletHeightmapCollisionShape(const Texture *heightmap, const Vector3 &scale);

    ~BulletHeightmapCollisionShape() override = default;

    /**
     * Get a handle to the bullet object.
     *
     * @returns
     *   Bullet object.
     */
    btCollisionShape *handle() const override;

  private:
    /** Bullet collision shape. */
    std::unique_ptr<btHeightfieldTerrainShape> shape_;

    /** Local copy of height data (bullet only doesn't make a copy) */
    std::vector<float> height_data_;
};

}
