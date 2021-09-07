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
};

}
