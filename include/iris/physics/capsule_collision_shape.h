#pragma once

#include <any>
#include <memory>

#include "core/real.h"
#include "physics/collision_shape.h"

namespace iris
{

/**
 * Implementation of CollisionShape for a capsule.
 */
class CapsuleCollisionShape : public CollisionShape
{
  public:
    /**
     * Construct new CapsuleCollisionShape
     * @param width
     *   Diameter of capsule.
     *
     * @param height
     *   Height of capsule.
     */
    CapsuleCollisionShape(real width, real height);

    ~CapsuleCollisionShape() override;

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
