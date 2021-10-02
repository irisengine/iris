#pragma once

#include <array>

#include "core/colour.h"
#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"

namespace iris
{

/**
 * Implementation of Light for a constant and uniform light without direction or
 * position.
 */
class AmbientLight : public Light
{
  public:
    /**
     * Construct a new AmbientLight.
     *
     * @param colour
     *   The colour of the light.
     */
    AmbientLight(const Colour &colour);

    ~AmbientLight() override = default;

    /**
     * Get the type of light.
     *
     * @returns
     *   Light type.
     */
    LightType type() const override;

    /**
     * Get the raw data for the light colour.
     *
     * @returns
     *   Raw data (as floats) for the colour.
     */
    std::array<float, 4u> colour_data() const override;

    /**
     * Unused by this light type.
     *
     * @returns
     *   Array of 0.0f values.
     */
    std::array<float, 4u> world_space_data() const override;

    /**
     * Unused by this light type.
     *
     * @returns
     *   Array of 0.0f values.
     */
    std::array<float, 3u> attenuation_data() const override;

    /**
     * Get the colour of the light.
     *
     * @param
     *   Light colour.
     */
    Colour colour() const;

    /**
     * Set the colour of the light.
     *
     * @param
     *   New light colour.
     */
    void set_colour(const Colour &colour);

  private:
    /** Colour of ambient light. */
    Colour colour_;
};

}
