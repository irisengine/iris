#include "graphics/render_entity.h"

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
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
RenderEntity::RenderEntity(Mesh mesh, const Vector3 &position)
    : RenderEntity(std::move(mesh), {position, {}, {1.0f}})
{
}

RenderEntity::RenderEntity(Mesh mesh, const Transform &transform)
    : RenderEntity(std::move(mesh), transform, Skeleton{})
{
}

RenderEntity::RenderEntity(
    Mesh mesh,
    const Transform &transform,
    Skeleton skeleton)
    : meshes_()
    , transform_(transform)
    , normal_()
    , wireframe_(false)
    , primitive_type_(PrimitiveType::TRIANGLES)
    , skeleton_(std::move(skeleton))
    , receive_shadow_(true)
{
    meshes_.emplace_back(std::move(mesh));
    normal_ = create_normal_transform(transform_.matrix());
}

RenderEntity::RenderEntity(
    std::vector<Mesh> meshes,
    const Transform &transform,
    Skeleton skeleton)
    : meshes_(std::move(meshes))
    , transform_(transform)
    , normal_()
    , wireframe_(false)
    , primitive_type_(PrimitiveType::TRIANGLES)
    , skeleton_(std::move(skeleton))
    , receive_shadow_(true)
{
    normal_ = create_normal_transform(transform_.matrix());
}

Vector3 RenderEntity::position() const
{
    return transform_.translation();
}

void RenderEntity::set_position(const Vector3 &position)
{
    transform_.set_translation(position);
    normal_ = create_normal_transform(transform_.matrix());
}

Quaternion RenderEntity::orientation() const
{
    return transform_.rotation();
}

void RenderEntity::set_orientation(const Quaternion &orientation)
{
    transform_.set_rotation(orientation);
    normal_ = create_normal_transform(transform_.matrix());
}

void RenderEntity::set_scale(const Vector3 &scale)
{
    transform_.set_scale(scale);
    normal_ = create_normal_transform(transform_.matrix());
}

Matrix4 RenderEntity::transform() const
{
    return transform_.matrix();
}

Matrix4 RenderEntity::normal_transform() const
{
    return normal_;
}

Mesh *RenderEntity::mesh() const
{
    return mesh_;
}

void RenderEntity::set_mesh(Mesh *mesh)
{
    mesh_ = mesh;
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

bool RenderEntity::receive_shadow() const
{
    return receive_shadow_;
}

void RenderEntity::set_receive_shadow(bool receive_shadow)
{
    receive_shadow_ = receive_shadow;
}

}
