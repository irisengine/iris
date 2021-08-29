#pragma once

#include <array>

#include "graphics/lights/light_type.h"

namespace iris
{

/**
 * Interface for a Light - something which provides luminance in a rendered
 * scene.
 */
class Light
{
  public:
    virtual ~Light() = default;

    /**
     * Get the type of light.
     *
     * @returns
     *   Light type.
     */
    virtual LightType type() const = 0;

    /**
     * Get the raw data for the light. What this represents is dependent on the
     * type light (e.g. colour, position, etc).
     *
     * @returns
     *   Raw data (as floats) for the light.
     */
    virtual std::array<float, 4u> data() const = 0;
};

}
