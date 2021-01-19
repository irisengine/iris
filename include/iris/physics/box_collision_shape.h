#pragma once

#include <any>
#include <memory>

#include "core/vector3.h"
#include "physics/collision_shape.h"

namespace iris
{

/**
 * Implementation of CollisionShape for a box.
 */
class BoxCollisionShape : public CollisionShape
{
  public:
    /**
     * Construct a new BoxCollisionShape
     *
     * @param half_size
     *   The extends from the center of the box which define its size.
     */
    explicit BoxCollisionShape(const Vector3 &half_size);

    ~BoxCollisionShape() override;

    /**
     * Get native handle for physics engine implementation of collision shape.
     *
     * @returns
     *   Physics engine native handle.
     */
    std::any native_handle() const override;

  private:
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
