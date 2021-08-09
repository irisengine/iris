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

    std::array<float, 4u> data() const override
    {
        return {};
    }
};
