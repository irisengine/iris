////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/single_entity.h"

class Entity
{
  public:
    Entity(const std::vector<iris::SingleEntity *> &entities);

    Entity(const std::vector<iris::SingleEntity *> &entities, const iris::Vector3 &centre);

    iris::Vector3 centre() const;

    iris::Transform transform() const;

    void set_transform(const iris::Transform &transform);

    void apply_transform(const iris::Transform &transform);

    bool intersects(const iris::Vector3 &origin, const iris::Vector3 &direction);

    std::vector<iris::SingleEntity *> entities() const;

  private:
    std::vector<iris::SingleEntity *> entities_;

    iris::Vector3 centre_;

    iris::Transform transform_;
};
