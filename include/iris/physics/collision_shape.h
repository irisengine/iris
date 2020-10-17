#pragma once

#include <any>

namespace iris
{

/**
 * This is an interface for 3D shape used for collision detection and
 * resolution.
 */
class CollisionShape
{
  public:
    virtual ~CollisionShape() = default;

    /**
     * Get native handle for physics engine implementation of collision shape.
     *
     * @returns
     *   Physics engine native handle.
     */
    virtual std::any native_handle() const = 0;
};

}
