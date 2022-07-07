////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/single_entity.h"

#include <string>
#include <string_view>

#include "core/camera_type.h"
#include "core/error_handling.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/render_entity.h"
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

SingleEntity::SingleEntity(const Mesh *mesh, const Vector3 &position, PrimitiveType primitive_type)
    : SingleEntity(mesh, {position, {}, {1.0f}}, primitive_type)
{
}

SingleEntity::SingleEntity(const Mesh *mesh, const Transform &transform, PrimitiveType primitive_type)
    : SingleEntity(mesh, transform, nullptr, primitive_type)
{
}

SingleEntity::SingleEntity(
    const Mesh *mesh,
    const Transform &transform,
    Skeleton *skeleton,
    PrimitiveType primitive_type)
    : RenderEntity(mesh, primitive_type)
    , transform_(transform)
    , normal_()
    , skeleton_(skeleton)
{
    ensure(mesh != nullptr, "must supply mesh");

    normal_ = create_normal_transform(transform_.matrix());
}

std::size_t SingleEntity::instance_count() const
{
    return 1u;
}

Vector3 SingleEntity::position() const
{
    return transform_.translation();
}

void SingleEntity::set_position(const Vector3 &position)
{
    transform_.set_translation(position);
    normal_ = create_normal_transform(transform_.matrix());
}

Quaternion SingleEntity::orientation() const
{
    return transform_.rotation();
}

void SingleEntity::set_orientation(const Quaternion &orientation)
{
    transform_.set_rotation(orientation);
    normal_ = create_normal_transform(transform_.matrix());
}

Vector3 SingleEntity::scale() const
{
    return transform_.scale();
}

void SingleEntity::set_scale(const Vector3 &scale)
{
    transform_.set_scale(scale);
    normal_ = create_normal_transform(transform_.matrix());
}

Matrix4 SingleEntity::transform() const
{
    return transform_.matrix();
}

void SingleEntity::set_transform(const Matrix4 &transform)
{
    transform_.set_matrix(transform);
    normal_ = create_normal_transform(transform_.matrix());
}

void SingleEntity::set_transform(const Transform &transform)
{
    set_transform(transform.matrix());
}

Matrix4 SingleEntity::normal_transform() const
{
    return normal_;
}

void SingleEntity::set_mesh(const Mesh *mesh)
{
    mesh_ = mesh;
}

Skeleton *SingleEntity::skeleton()
{
    return skeleton_;
}

const Skeleton *SingleEntity::skeleton() const
{
    return skeleton_;
}

}
