////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/instanced_entity.h"

#include <vector>

#include "core/error_handling.h"
#include "core/matrix4.h"
#include "core/transform.h"
#include "graphics/mesh.h"
#include "graphics/render_entity_type.h"

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

InstancedEntity::InstancedEntity(const Mesh *mesh, const std::vector<Transform> &instances)
    : RenderEntity(mesh, PrimitiveType::TRIANGLES)
    , instance_count_(instances.size())
{
    ensure(instances.size() > 1, "must have at least two instances");

    for (const auto &instance : instances)
    {
        data_.emplace_back(instance.matrix());
        data_.emplace_back(create_normal_transform(instance.matrix()));
    }
}

RenderEntityType InstancedEntity::type() const
{
    return RenderEntityType::INSTANCED;
}

const std::vector<Matrix4> &InstancedEntity::data() const
{
    return data_;
}

bool InstancedEntity::has_transparency() const
{
    return false;
}

std::size_t InstancedEntity::instance_count() const
{
    return instance_count_;
}

}
