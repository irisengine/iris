////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "entity.h"

#include <algorithm>
#include <numeric>
#include <string_view>
#include <vector>

#include "core/error_handling.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/single_entity.h"

Entity::Entity(const std::vector<iris::SingleEntity *> &entities, std::string_view file_name)
    : entities_(entities)
    , transform_()
    , file_name_(file_name)
{
}

iris::Transform Entity::transform() const
{
    return transform_;
}

void Entity::set_transform(const iris::Transform &transform)
{
    for (auto *entity : entities_)
    {
        entity->set_transform(transform.matrix());
    }

    transform_ = transform;
}

void Entity::apply_transform(const iris::Transform &transform)
{
    for (auto *entity : entities_)
    {
        entity->set_transform(entity->transform() * transform.matrix());
    }

    transform_ *= transform;
}

bool Entity::intersects(const iris::Vector3 &origin, const iris::Vector3 &direction)
{
    const auto radius = std::numbers::sqrt2_v<float>;
    auto intersects = false;

    for (auto *entity : entities_)
    {
        const auto centre = entity->position();

        const auto b = direction.dot(origin - centre);
        const auto c = (origin - centre).dot(origin - centre) - std::pow(radius, 2.0f);

        const auto t = (std::pow(b, 2.0f) - c);

        if (t >= 0.0f)
        {
            intersects = true;
            break;
        }
    }

    return intersects;
}

std::vector<iris::SingleEntity *> Entity::entities() const
{
    return entities_;
}

std::string Entity::file_name() const
{
    return file_name_;
}
