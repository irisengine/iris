#pragma once

#include <vector>

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/primitive_type.h"
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
     * @param mesh
     *   Mesh to render.
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
     * @param material
     *   Pointer to material to render mesh.
     *
     * @param wireframe
     *   Should render as wireframe.
     *
     * @param camera_type
     *   Type of camera to render with.
     */
    RenderEntity(
        std::vector<Mesh> meshes,
        const Vector3 &position,
        const Quaternion &orientation,
        const Vector3 &scale,
        Material *material,
        bool wireframe,
        CameraType camera_type);

    virtual ~RenderEntity() = 0;

    /** Disabled */
    RenderEntity(const RenderEntity &) = delete;
    RenderEntity &operator=(const RenderEntity &) = delete;

    /**
     * Set the position of the RenderEntity.
     *
     * @param position
     *   New position.
     */
    void set_position(const Vector3 &position);

    /**
     * Set the orientation of the RenderEntity.
     *
     * @param orientation
     *   New oritentation.
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
     * Set the mesh of the RenderEntity.
     *
     * @param mesh
     *   New mesh.
     */
    void set_meshes(std::vector<Mesh> meshes);

    /**
     * Get the transformation matrix of the RenderEntity.
     *
     * @returns
     *   Transformation matrix.
     */
    Matrix4 transform() const;

    /**
     * Get the transformation matrix for the normals of the RenderEntity.
     *
     * @returns
     *   Normal transformation matrix.
     */
    Matrix4 normal_transform() const;

    /**
     * Get a const reference to the Mesh that make up the RenderEntity.
     *
     * @returns
     *   Const reference to mesh.
     */
    const std::vector<Mesh> &meshes() const;

    /**
     * Get a const reference to the RenderEntitys rendering material.
     *
     * @returns
     *   Const reference to material.
     */
    const Material &material() const;

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
     * Get camera type.
     *
     * @returns
     *   Type of camera.
     */
    CameraType camera_type() const;

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

  protected:
    /** Collection of meshes. */
    std::vector<Mesh> meshes_;

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

    /** Material to render with. */
    Material *material_;

    /** Whether the object should be rendered as a wireframe. */
    bool wireframe_;

    /** Type of camera. */
    CameraType camera_type_;

    /** Primitive type. */
    PrimitiveType primitive_type_;
};

}
