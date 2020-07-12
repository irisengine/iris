#pragma once

#include <any>
#include <memory>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/rigid_body.h"

namespace eng
{

/**
 * Implementation of a RigidBody for a box shape.
 */
class BoxRigidBody : public RigidBody
{
    public:

        /**
         * Construct a new BoxRigidBody.
         *
         * @param position
         *   Position of centre of box.
         *
         * @param half_size
         *   The extends from the center of the box which define its size.
         *
         * @param is_static
         *   If the box is a static rigid body i.e. doesn't move but can be
         *   collided with.
         */
        BoxRigidBody(
            const Vector3 &position,
            const Vector3 &half_size,
            bool is_static);

        ~BoxRigidBody() override;

        /**
         * Get position of rigid body centre of mass.
         *
         * @returns
         *   Rigid body position.
         */
        Vector3 position() const override;

        /**
         * Get orientation of rigid body.
         *
         * @returns
         *   Rigid body orientation.
         */
        Quaternion orientation() const override;

        /**
         * Get native handle for physics engine implementation of rigid body.
         *
         * @returns
         *   Physics engine native handle.
         */
        std::any native_handle() const override;

    private:

        /** Physics API implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

