#pragma once

#include <any>
#include <memory>

#include "physics/rigid_body.h"

namespace eng
{

/**
 * Implementation of a RigidBody for a box shape.
 */
class CapsuleRigidBody : public RigidBody
{
    public:

        /**
         * Construct a new CapsuleRigidBody.
         *
         * @param position
         *   Position of centre of capsule.
         *
         * @param width
         *   Diameter of capsule.
         *
         * @param height
         *   Height of capsule.
         *
         * @param is_static
         *   If the box is a static rigid body i.e. doesn't move but can be
         *   collided with.
         */
        CapsuleRigidBody(
            const Vector3 &position,
            real width,
            real height,
            bool is_static);

        ~CapsuleRigidBody() override;

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

