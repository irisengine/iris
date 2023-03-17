////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>

#include "core/camera.h"
#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/lights/light.h"

namespace iris
{

/**
 * An implementation of Light for a directional light. This is a light
 * infinitely far away from the scene and consistent in all directions.
 *
 * A light may be constructed to cast shadows, this will cause extra render
 * passes to be created which can impact performance (depending on scene
 * complexity).
 */
class DirectionalLight : public Light
{
  public:
    /** Create a new DirectionalLight (white).
     *
     * @param direction
     *   The direction the rays of light are pointing, for examples to have a
     *   light shining directly down on a scene then its direction would be
     *   (0, -1, 0).
     *
     * @param cast_shadows
     *   True if this light should generate shadows, false otherwise.
     */
    DirectionalLight(const Vector3 &direction, bool cast_shadows = false);

    /** Create a new DirectionalLight.
     *
     * @param direction
     *   The direction the rays of light are pointing, for examples to have a
     *   light shining directly down on a scene then its direction would be
     *   (0, -1, 0).
     *
     * @param colour
     *   The colour of the light.
     *
     * @param cast_shadows
     *   True if this light should generate shadows, false otherwise.
     */
    DirectionalLight(const Vector3 &direction, const Colour &colour, bool cast_shadows = false);

    ~DirectionalLight() override = default;

    /**
     * Get the type of light.
     *
     * @returns
     *   Light type.
     */
    LightType type() const override;

    /**
     * Unused by this light type.
     *
     * @returns
     *   Array of 1.0f values.
     */
    std::array<float, 4u> colour_data() const override;

    /**
     * Get the raw data for the lights world space property i.e direction.
     *
     * @returns
     *   Raw data (as floats) for the world space property.
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
     * Get direction of light.
     *
     * @returns
     *   Light direction.
     */
    Vector3 direction() const;

    /**
     * Set direction of light.
     *
     * @param direction
     *   New light direction.
     */
    void set_direction(const Vector3 &direction);

    /**
     * Check if this light should cast shadows.
     *
     * @returns
     *   True if light casts shadows, false otherwise.
     */
    bool casts_shadows() const;

    /**
     * Get the camera used for rendering the shadow map for this light.
     *
     * This is used internally and should not normally be called manually.
     *
     * @returns
     *   Shadow map camera.
     */
    const Camera &shadow_camera() const;

  private:
    /** Light direction. */
    Vector3 direction_;

    /** Shadow map camera. */
    Camera shadow_camera_;

    /** Should shadows be generated. */
    bool cast_shadows_;

    /** Light colour. */
    Colour colour_;
};

}
