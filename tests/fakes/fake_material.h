////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "graphics/cube_map.h"
#include "graphics/material.h"
#include "graphics/texture.h"

class FakeMaterial : public iris::Material
{
  public:
    ~FakeMaterial() override = default;

    std::vector<iris::Texture *> textures() const override
    {
        return {};
    }

    const iris::CubeMap *cube_map() const override
    {
        return nullptr;
    }
};
