#pragma once

#include <memory>
#include <vector>

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/material.h"
#include "graphics/primitive_type.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * A base class for a renderable entity, i.e. one that can be added to a
 * render system. It encapsulates a mesh, material and world translation.
 */
class RenderEntity
{
  public:
    /**
     * Construct a RenderEntity.
     *
     * @param buffer_descriptor
     *   BufferDescriptor for mesh to render.
     *
     * @param position
     *   Centre of mesh in world space.
     *
     * @param scale
     *   Scale of mesh.
     */
    RenderEntity(
        BufferDescriptor buffer_descriptor,
        const Vector3 &position,
        const Vector3 &scale);

    /**
     * Construct a RenderEntity.
     *
     * @param buffer_descriptor
     *   BufferDescriptor for mesh to render.
     *
     * @param position
     *   Centre of mesh in world space.
     *
     * @param orientation
     *   Orientation of mesh.
     *
     * @param scale
     *   Scale of mesh.
     */
    RenderEntity(
        BufferDescriptor buffer_descriptor,
        const Vector3 &position,
        const Quaternion &orientation,
        const Vector3 &scale);

    /**
     * Construct a RenderEntity.
     *
     * @param buffer_descriptor
     *   BufferDescriptor for mesh to render.
     *
     * @param position
     *   Centre of mesh in world space.
     *
     * @param orientation
     *   Orientation of mesh.
     *
     * @param scale
     *   Scale of mesh.
     *
     * @param skeleton
     *   Skeleton.
     */
    RenderEntity(
        BufferDescriptor buffer_descriptor,
        const Vector3 &position,
        const Quaternion &orientation,
        const Vector3 &scale,
        Skeleton skeleton);

    /**
     * Construct a RenderEntity.
     *
     * @param buffer_descriptors
     *   Collection of BufferDescriptors for meshes to render.
     *
     * @param position
     *   Centre of mesh in world space.
     *
     * @param orientation
     *   Orientation of mesh.
     *
     * @param scale
     *   Scale of mesh.
     *
     * @param skeleton
     *   Skeleton.
     */
    RenderEntity(
        std::vector<BufferDescriptor> buffer_descriptor,
        const Vector3 &position,
        const Quaternion &orientation,
        const Vector3 &scale,
        Skeleton skeleton);

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
     * Get all BufferDescriptors for this entity.
     *
     * @returns
     *   Collection of buffer descriptors.
     */
    const std::vector<BufferDescriptor> &buffer_descriptors() const;

    /**
     * Set buffer descriptor.
     *
     * @param buffer_descriptor.
     *   New (single) buffer descriptor.
     */
    void set_buffer_descriptors(BufferDescriptor buffer_descriptor);

    /**
     * Set buffer descriptors.
     *
     * @param buffer_descriptor.
     *   New buffer descriptors.
     */
    void set_buffer_descriptors(
        std::vector<BufferDescriptor> buffer_descriptors);

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
     * Set primitive type.
     *
     * @param type
     *   New primitive type.
     */
    void set_primitive_type(PrimitiveType type);

    /**
     * Get reference to skeleton.
     *
     * @returns
     *   Reference to skeleton.
     */
    Skeleton &skeleton();

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

  private:
    /** BufferDescriptors to render. */
    std::vector<BufferDescriptor> buffer_descriptors_;

    /** The position of the RenderEntity. */
    Vector3 position_;

    /** The orientation of the RenderEntity. */
    Quaternion orientation_;

    /** The scale of the RenderEntity. */
    Vector3 scale_;

    /** Model transformation matrix4. */
    Matrix4 model_;

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
};

}
