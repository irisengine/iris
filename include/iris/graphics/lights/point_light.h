#pragma once

#include "core/vector3.h"

namespace iris
{

/**
 * A light emitting uniformly from a point in 3D space.
 */
class PointLight
{
  public:
    /**
     * Create a new PointLight.
     *
     * @param position
     *   Position of light in 3D space.
     */
    PointLight(const Vector3 &position);

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
