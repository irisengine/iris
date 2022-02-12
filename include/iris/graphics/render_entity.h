////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

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
#include "graphics/render_graph/render_node.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * A renderable entity.
 */
class RenderEntity
{
  public:
    /**
     * Construct a RenderEntity.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param position
     *   Centre of mesh in world space.
     *
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    RenderEntity(const Mesh *mesh, const Vector3 &position, PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    /**
     * Construct a RenderEntity.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param transform
     *   Transform of entity in world space.
     *
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    RenderEntity(const Mesh *mesh, const Transform &transform, PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    /**
     * Construct a RenderEntity.
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
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    RenderEntity(
        const Mesh *mesh,
        const Transform &transform,
        Skeleton skeleton,
        PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    RenderEntity(const RenderEntity &) = delete;
    RenderEntity &operator=(const RenderEntity &) = delete;
    RenderEntity(RenderEntity &&) = default;
    RenderEntity &operator=(RenderEntity &&) = default;

    /**
     * Get position.
     *
     * @returns
     *   Position.
     */
    Vector3 position() const;

    /**
     * Set the position of the RenderEntity.
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
     * Set the orientation of the RenderEntity.
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
     * Set the scale of the RenderEntity.
     *
     * @param scale
     *   New scale.
     */
    void set_scale(const Vector3 &scale);

    /**
     * Get the transformation matrix of the RenderEntity.
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
     * Get the transformation matrix for the normals of the RenderEntity.
     *
     * @returns
     *   Normal transformation matrix.
     */
    Matrix4 normal_transform() const;

    /**
     * Get all Mesh for this entity.
     *
     * @returns
     *   Mesh.
     */
    const Mesh *mesh() const;

    /**
     * Set Mesh.
     *
     * @param mesh
     *   New Mesh.
     */
    void set_mesh(const Mesh *mesh);

    /**
     * Returns whether the object should be rendered as a wireframe.
     *
     * @returns
     *   True if should be rendered as a wireframe, false otherwise.
     */
    bool should_render_wireframe() const;

    /**
     * Sets whether the object should be rendered as a wireframe.
     *
     * @param wrireframe
     *   True if should be rendered as a wireframe, false otherwise.
     */
    void set_wireframe(const bool wireframe);

    /**
     * Get primitive type.
     *
     * @returns
     *   Primitive type.
     */
    PrimitiveType primitive_type() const;

    /**
     * Get reference to skeleton.
     *
     * @returns
     *   Reference to skeleton.
     */
    Skeleton &skeleton();

    /**
     * Get const reference to skeleton.
     *
     * @returns
     *   Reference to skeleton.
     */
    const Skeleton &skeleton() const;

    /**
     * Can this entity have shadows rendered on it.
     *
     * @returns
     *   True if shadows should be rendered, false otherwise.
     */
    bool receive_shadow() const;

    /**
     * Set whether this object can have shadows rendered on it.
     *
     * @param receive_shadow
     *   New receive shadow option.
     */
    void set_receive_shadow(bool receive_shadow);

    /**
     * Get (optional) name.
     *
     * @return
     *  Name of entity, empty string if not set.
     */
    std::string name() const;

    /**
     * Set the name of the entity.
     *
     * @param name
     *   New name.
     */
    void set_name(std::string_view name);

  private:
    /** Mesh to render. */
    const Mesh *mesh_;

    /** World space transform. */
    Transform transform_;

    /** Normal transformation matrix. */
    Matrix4 normal_;

    /** Whether the object should be rendered as a wireframe. */
    bool wireframe_;

    /** Primitive type. */
    PrimitiveType primitive_type_;

    /** Skeleton. */
    Skeleton skeleton_;

    /** Should object render shadows. */
    bool receive_shadow_;

    /** Optional name (default is empty string). */
    std::string name_;
};

}
