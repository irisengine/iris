#include "graphics/render_entity.h"

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "graphics/material_factory.h"
#include "graphics/mesh.h"
#include "graphics/shape_factory.h"

namespace
{

/**
 * Helper function to create a normal transformation matrix from a model
 * matrix.
 *
 * @param model
 *   The model matrix to calculate from.
 *
 * @returns
 *   Normal transformation matrix.
 */
iris::Matrix4 create_normal_transform(const iris::Matrix4 &model)
{
    auto normal = iris::Matrix4::transpose(iris::Matrix4::invert(model));
    
    // remove the translation components
    normal[3] = 0.0f;
    normal[7] = 0.0f;
    normal[11] = 0.0f;

    return normal;
}

}

namespace iris
{

RenderEntity::RenderEntity(
    Mesh mesh,
    const Vector3 &position,
    const Quaternion &orientation,
    const Vector3 &scale,
    Material *material,
    bool wireframe,
    CameraType camera_type)
    : mesh_(std::move(mesh)),
      position_(position),
      orientation_(orientation),
      scale_(scale),
      model_(),
      normal_(),
      material_(material),
      wireframe_(wireframe),
      camera_type_(camera_type)
{
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) * Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

RenderEntity::~RenderEntity() = default;

void RenderEntity::set_position(const Vector3 &position)
{
    position_ = position;

    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) * Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

void RenderEntity::set_orientation(const Quaternion &orientation)
{
    orientation_ = orientation;
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) * Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

void RenderEntity::set_scale(const Vector3 &scale)
{
    scale_ = scale;
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) * Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

void RenderEntity::set_mesh(Mesh mesh)
{
    mesh_ = std::move(mesh);
}

Matrix4 RenderEntity::transform() const
{
    return model_;
}

Matrix4 RenderEntity::normal_transform() const
{
    return normal_;
}

const Mesh& RenderEntity::mesh() const
{
    return mesh_;
}

const Material& RenderEntity::material() const
{
    return *material_;
}

bool RenderEntity::should_render_wireframe() const
{
    return wireframe_;
}

void RenderEntity::set_wireframe(const bool wireframe)
{
    wireframe_ = wireframe;
}

CameraType RenderEntity::camera_type() const
{
    return camera_type_;
}

}


