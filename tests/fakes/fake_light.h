////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <limits>

#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"

class FakeLight : public iris::Light
{
  public:
    ~FakeLight() override = default;

    iris::LightType type() const override
    {
        return static_cast<iris::LightType>(
            std::numeric_limits<iris::LightType>::max());
    }

    std::array<float, 4u> colour_data() const override
    {
        return {};
    }

    std::array<float, 4u> world_space_data() const override
    {
        return {};
    }

    std::array<float, 3u> attenuation_data() const override
    {
        return {};
    }
};
