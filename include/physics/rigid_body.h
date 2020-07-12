#pragma once

#include <any>

#include "core/quaternion.h"
#include "core/vector3.h"

namespace eng
{

/**
 * Interface for a rigid body. A rigid body is a physics entity that can be
 * added to the physics systems and simulated. It will collide and interact
 * with other rigid bodies.
 */
class RigidBody
{
    public:

        // default
        virtual ~RigidBody() = default;

        /**
         * Get position of rigid body centre of mass.
         *
         * @returns
         *   Rigid body position.
         */
        virtual Vector3 position() const = 0;

        /**
         * Get orientation of rigid body.
         *
         * @returns
         *   Rigid body orientation.
         */
        virtual Quaternion orientation() const = 0;

        /**
         * Get native handle for physics engine implementation of rigid body.
         *
         * @returns
         *   Physics engine native handle.
         */
        virtual std::any native_handle() const = 0;
};

}

