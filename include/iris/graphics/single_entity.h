////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "graphics/mesh.h"
#include "graphics/primitive_type.h"
#include "graphics/render_entity.h"
#include "graphics/render_entity_type.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Implementation of RenderEntity for a single instance mesh.
 */
class SingleEntity : public RenderEntity
{
  public:
    /**
     * Construct a SingleEntity.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param position
     *   Centre of mesh in world space.
     *
     * @param has_transparency
     *   Whether entity will have transparency when rendered.
     *
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    SingleEntity(
        const Mesh *mesh,
        const Vector3 &position,
        bool has_transparency = false,
        PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    /**
     * Construct a SingleEntity.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param transform
     *   Transform of entity in world space.
     *
     * @param has_transparency
     *   Whether entity will have transparency when rendered.
     *
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    SingleEntity(
        const Mesh *mesh,
        const Transform &transform,
        bool has_transparency = false,
        PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    /**
     * Construct a SingleEntity.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param transform
     *   Transform of entity in world space.
     *
     * @param skeleton
     *   Skeleton.
     *
     * @param has_transparency
     *   Whether entity will have transparency when rendered.
     *
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    SingleEntity(
        const Mesh *mesh,
        const Transform &transform,
        Skeleton *skeleton,
        bool has_transparency = false,
        PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    ~SingleEntity() override = default;

    SingleEntity(const SingleEntity &) = delete;
    SingleEntity &operator=(const SingleEntity &) = delete;
    SingleEntity(SingleEntity &&) = default;
    SingleEntity &operator=(SingleEntity &&) = default;

    /**
     * Get entity type.
     *
     * @returns
     *   Entity type.
     */
    RenderEntityType type() const override;

    /**
     * Get position.
     *
     * @returns
     *   Position.
     */
    Vector3 position() const;

    /**
     * Set the position of the SingleEntity.
     *
     * @param position
     *   New position.
     */
    void set_position(const Vector3 &position);

    /**
     * Get orientation.
     *
     * @return
     *   Orientation.
     */
    Quaternion orientation() const;

    /**
     * Set the orientation of the SingleEntity.
     *
     * @param orientation
     *   New orientation.
     */
    void set_orientation(const Quaternion &orientation);

    /**
     * Get scale.
     *
     * @return
     *   Scale.
     */
    Vector3 scale() const;

    /**
     * Set the scale of the SingleEntity.
     *
     * @param scale
     *   New scale.
     */
    void set_scale(const Vector3 &scale);

    /**
     * Get the transformation matrix of the SingleEntity.
     *
     * @returns
     *   Transformation matrix.
     */
    Matrix4 transform() const;

    /**
     * Set transformation matrix.
     *
     * @param transform
     *   New transform matrix.
     */
    void set_transform(const Matrix4 &transform);

    /**
     * Set transformation.
     *
     * @param transform
     *   New transform.
     */
    void set_transform(const Transform &transform);

    /**
     * Get the transformation matrix for the normals of the SingleEntity.
     *
     * @returns
     *   Normal transformation matrix.
     */
    Matrix4 normal_transform() const;

    /**
     * Set Mesh.
     *
     * @param mesh
     *   New Mesh.
     */
    void set_mesh(const Mesh *mesh);

    /**
     * Get pointer to skeleton.
     *
     * @returns
     *   Pointer to skeleton.
     */
    Skeleton *skeleton();

    /**
     * Get const pointer to skeleton.
     *
     * @returns
     *   Pointer to skeleton.
     */
    const Skeleton *skeleton() const;

    /**
     * Get if entity will be rendered with transparency.
     *
     * @returns
     *   True if entity will have transparency, otherwise false.
     */
    bool has_transparency() const override;

  private:
    /** World space transform. */
    Transform transform_;

    /** Normal transformation matrix. */
    Matrix4 normal_;

    /** Skeleton. */
    Skeleton *skeleton_;

    /** Flag indicating entity will have transparency. */
    bool has_transparency_;
};

}
