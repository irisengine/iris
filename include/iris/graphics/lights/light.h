#pragma once

#include <array>

#include "graphics/lights/light_type.h"

namespace iris
{

/**
 * Interface for a Light - something which provides luminance in a rendered
 * scene.
 *
 * Whilst this interface defines methods for getting various properties, they
 * may not all be valid for all light types. In that case the return value
 * should be considered unspecified.
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
     * Get the raw data for the light colour.
     *
     * @returns
     *   Raw data (as floats) for the colour.
     */
    virtual std::array<float, 4u> colour_data() const = 0;

    /**
     * Get the raw data for the lights world space property e.g. position or
     * direction.
     *
     * @returns
     *   Raw data (as floats) for the world space property.
     */
    virtual std::array<float, 4u> world_space_data() const = 0;

    /**
     * Get the raw data for the lights attenuation.
     *
     * @returns
     *   Raw data (as floats) for the attenuation.
     */
    virtual std::array<float, 3u> attenuation_data() const = 0;
};

}
