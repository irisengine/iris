////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

#include "core/vector3.h"
#include "physics/bullet/bullet_collision_shape.h"

namespace iris
{

class Mesh;

/**
 * Implementation of CollisionShape for a mesh.
 */
class BulletMeshCollisionShape : public BulletCollisionShape
{
  public:
    /**
     * Construct a new BMeshCollisionShape
     *
     * @param mesh
     *   The mesh to create the collision shape from.
     *
     * @param scale
     *   The scale of the model as it will be rendered.
     */
    explicit BulletMeshCollisionShape(const Mesh *mesh, const Vector3 &scale);

    ~BulletMeshCollisionShape() override = default;

    /**
     * Get a handle to the bullet object.
     *
     * @returns
     *   Bullet object.
     */
    btCollisionShape *handle() const override;

  private:
    /** Bullet collision shape. */
    std::unique_ptr<btBvhTriangleMeshShape> shape_;

    /** Bullet object which stores pointers to vertex and index data. */
    std::unique_ptr<btTriangleIndexVertexArray> triangle_index_vertex_data_;
};

}
