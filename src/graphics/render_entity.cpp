#include "graphics/render_entity.h"

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "graphics/mesh_factory.h"
#include "graphics/skeleton.h"

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
    BufferDescriptor buffer_descriptor,
    const Vector3 &position,
    const Vector3 &scale)
    : RenderEntity(std::move(buffer_descriptor), position, {}, scale)
{
}

RenderEntity::RenderEntity(
    BufferDescriptor buffer_descriptor,
    const Vector3 &position,
    const Quaternion &orientation,
    const Vector3 &scale)
    : RenderEntity(
          std::move(buffer_descriptor),
          position,
          orientation,
          scale,
          Skeleton{})
{
}

RenderEntity::RenderEntity(
    BufferDescriptor buffer_descriptor,
    const Vector3 &position,
    const Quaternion &orientation,
    const Vector3 &scale,
    Skeleton skeleton)
    : buffer_descriptors_()
    , position_(position)
    , orientation_(orientation)
    , scale_(scale)
    , model_()
    , normal_()
    , wireframe_(false)
    , primitive_type_(PrimitiveType::TRIANGLES)
    , skeleton_(std::move(skeleton))
{
    buffer_descriptors_.emplace_back(std::move(buffer_descriptor));
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) *
             Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

RenderEntity::RenderEntity(
    std::vector<BufferDescriptor> buffer_descriptors,
    const Vector3 &position,
    const Quaternion &orientation,
    const Vector3 &scale,
    Skeleton skeleton)
    : buffer_descriptors_(std::move(buffer_descriptors))
    , position_(position)
    , orientation_(orientation)
    , scale_(scale)
    , model_()
    , normal_()
    , wireframe_(false)
    , primitive_type_(PrimitiveType::TRIANGLES)
    , skeleton_(std::move(skeleton))
{
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) *
             Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

Vector3 RenderEntity::position() const
{
    return position_;
}

void RenderEntity::set_position(const Vector3 &position)
{
    position_ = position;

    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) *
             Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

Quaternion RenderEntity::orientation() const
{
    return orientation_;
}

void RenderEntity::set_orientation(const Quaternion &orientation)
{
    orientation_ = orientation;
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) *
             Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

void RenderEntity::set_scale(const Vector3 &scale)
{
    scale_ = scale;
    model_ = Matrix4::make_translate(position_) * Matrix4(orientation_) *
             Matrix4::make_scale(scale_);
    normal_ = create_normal_transform(model_);
}

Matrix4 RenderEntity::transform() const
{
    return model_;
}

Matrix4 RenderEntity::normal_transform() const
{
    return normal_;
}

const std::vector<BufferDescriptor> &RenderEntity::buffer_descriptors() const
{
    return buffer_descriptors_;
}

bool RenderEntity::should_render_wireframe() const
{
    return wireframe_;
}

void RenderEntity::set_wireframe(const bool wireframe)
{
    wireframe_ = wireframe;
}

PrimitiveType RenderEntity::primitive_type() const
{
    return primitive_type_;
}

void RenderEntity::set_primitive_type(PrimitiveType type)
{
    primitive_type_ = type;
}

Skeleton &RenderEntity::skeleton()
{
    return skeleton_;
}

}
