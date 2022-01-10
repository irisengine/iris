////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/bullet_mesh_collision_shape.h"

#include <memory>

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

#include "core/error_handling.h"
#include "graphics/mesh.h"
#include "graphics/vertex_data.h"

namespace iris
{

BulletMeshCollisionShape::BulletMeshCollisionShape(const Mesh *mesh, const Vector3 &scale)
    : shape_(nullptr)
    , triangle_index_vertex_data_(nullptr)
{
    const auto &mesh_index_data = mesh->indices();
    const auto &mesh_vertex_data = mesh->vertices();
    const auto num_faces = static_cast<int>(mesh_index_data.size()) / 3;

    // get pointers to the vertex and index data
    // annoyingly bullet expects this data as non-const int pointers, so we needed a bit of casting to massage our data
    // into the expected format
    auto *index_ptr = reinterpret_cast<int *>(const_cast<std::uint32_t *>(mesh_index_data.data()));
    auto *vertex_ptr = reinterpret_cast<float *>(const_cast<VertexData *>(mesh_vertex_data.data()));

    triangle_index_vertex_data_ = std::make_unique<btTriangleIndexVertexArray>(
        num_faces,
        index_ptr,
        sizeof(int) * 3,
        static_cast<int>(mesh_vertex_data.size()),
        vertex_ptr,
        sizeof(VertexData));

    shape_ = std::make_unique<btBvhTriangleMeshShape>(triangle_index_vertex_data_.get(), true);
    shape_->setLocalScaling({scale.x, scale.y, scale.z});
}

btCollisionShape *BulletMeshCollisionShape::handle() const
{
    return shape_.get();
}

}
