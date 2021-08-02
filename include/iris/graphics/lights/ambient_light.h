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
     * Get the raw data of the light. This is the colour.
     *
     * @returns
     *   Colour as raw data.
     */
    std::array<float, 4u> data() const override;

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
    Colour colour_;
};

}
