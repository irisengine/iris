////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "entity.h"

#include <algorithm>
#include <numeric>
#include <vector>

#include "core/error_handling.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/single_entity.h"

namespace
{

iris::Vector3 calculate_centre(const std::vector<iris::SingleEntity *> &entities)
{
    iris::ensure(!entities.empty(), "vector cannot be empty");

    return std::reduce(
               std::cbegin(entities),
               std::cend(entities),
               iris::Vector3{},
               [](const auto &total, const iris::SingleEntity *e2) -> iris::Vector3
               { return total + e2->position(); }) /
           static_cast<float>(entities.size());
}

}

Entity::Entity(const std::vector<iris::SingleEntity *> &entities)
    : Entity(entities, calculate_centre(entities))
{
}

Entity::Entity(const std::vector<iris::SingleEntity *> &entities, const iris::Vector3 &centre)
    : entities_(entities)
    , centre_(centre)
    , transform_()
{
}

iris::Vector3 Entity::centre() const
{
    return centre_;
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
