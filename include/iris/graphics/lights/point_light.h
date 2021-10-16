////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>

#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"

namespace iris
{

/**
 * Implementation of Light for a light emitting uniformly from a point in 3D
 * space.
 *
 * For this light attenuation is calculated as:
 * 1.0 / (constant + (linear * d) + (quadratic * d * d))
 *
 * WHere d is the distance of the fragment to the light source.
 */
class PointLight : public Light
{
  public:
    /**
     * Create a new white PointLight.
     *
     * @param position
     *   Position of light in 3D space.
     */
    PointLight(const Vector3 &position);

    /**
     * Create a new PointLight.
     *
     * @param position
     *   Position of light in 3D space.
     *
     * @param colour
     *   Colour of the light.
     */
    PointLight(const Vector3 &position, const Colour &colour);

    ~PointLight() override = default;

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
     * Get the raw data for the lights world space property i.e position.
     *
     * @returns
     *   Raw data (as floats) for the world space property.
     */
    std::array<float, 4u> world_space_data() const override;

    /**
     * Get the raw data for the lights attenuation.
     *
     * @returns
     *   Raw data (as floats) for the attenuation.
     */
    std::array<float, 3u> attenuation_data() const override;

    /**
     * Get position of light.
     *
     * @returns
     *   Light position.
     */
    Vector3 position() const;

    /**
     * Set light position.
     *
     * @param position
     *   New position.
     */
    void set_position(const Vector3 &position);

    /**
     * Get light colour.
     *
     * @returns
     *   Light colour.
     */
    Colour colour() const;

    /**
     * Set light colour.
     *
     * @param colour
     *   New colour.
     */
    void set_colour(const Colour &colour);

    /**
     * Get constant attenuation term.
     *
     * @returns
     *   Constant attenuation term.
     */
    float attenuation_constant_term() const;

    /**
     * Set constant attenuation term.
     *
     * @param constant
     *   New constant term.
     */
    void set_attenuation_constant_term(float constant);

    /**
     * Get linear attenuation term.
     *
     * @returns
     *   Linear attenuation term.
     */
    float attenuation_linear_term() const;

    /**
     * Set linear attenuation term.
     *
     * @param linear
     *   New linear term.
     */
    void set_attenuation_linear_term(float linear);

    /**
     * Get quadratic attenuation term.
     *
     * @returns
     *   Quadratic attenuation term.
     */
    float attenuation_quadratic_term() const;

    /**
     * Set quadratic attenuation term.
     *
     * @param quadratic
     *   New quadratic term.
     */
    void set_attenuation_quadratic_term(float quadratic);

  private:
    /**
     * Struct storing all attenuation terms. This makes it convenient to memcpy
     */
    struct AttenuationTerms
    {
        float constant;
        float linear;
        float quadratic;
    };

    /** Light position. */
    Vector3 position_;

    /** Light colour. */
    Colour colour_;

    /** Attenuation terms. */
    AttenuationTerms attenuation_terms_;
};

}
