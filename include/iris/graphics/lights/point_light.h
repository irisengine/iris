#pragma once

#include <array>

#include "core/vector3.h"
#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"

namespace iris
{

/**
 * Implementation of Light for a light emitting uniformly from a point in 3D
 * space.
 */
class PointLight : public Light
{
  public:
    /**
     * Create a new PointLight.
     *
     * @param position
     *   Position of light in 3D space.
     */
    PointLight(const Vector3 &position);

    ~PointLight() override = default;

    /**
     * Get the type of light.
     *
     * @returns
     *   Light type.
     */
    LightType type() const override;

    /**
     * Get the raw data of the light. This is the position.
     *
     * @returns
     *   Position as raw data.
     */
    std::array<float, 4u> data() const override;

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

  private:
    /** Light position. */
    Vector3 position_;
};

}
