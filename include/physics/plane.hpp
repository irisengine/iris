#pragma once

#include "rigid_body.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Implementation of rigid_body for a plane shaped object. The plane is infinite
 * in all directions, with it's centre at the world origin. It is specified by a
 * normal and an offset. The offset is a scalar value which specifies how far
 * the centre of the plane is offset along the normal.
 *
 * Planes are always static.
 */
class plane final : public rigid_body
{
    public:

        /**
         * Construct a new plane.
         *
         * @param normal
         *   Normal of the plane.
         *
         * @param offset
         *   Offset along normal of the plane.
         */
        plane(
            const vector3 &normal,
            const float offset);

        /** Default */
        ~plane() override = default;
        plane(const plane&) = default;
        plane& operator=(const plane&) = default;
        plane(plane&&) = default;
        plane& operator=(plane&&) = default;

        /**
         * Get the plane normal.
         *
         * @returns
         *   Plane normal.
         */
        vector3 normal() const noexcept;

        /**
         * Get the plane offset.
         *
         * @returns
         *   Offset.
         */
        float offset() const noexcept;

    private:

        /** Plane normal. */
        vector3 normal_;

        /** Plane offset. */
        float offset_;
};

}

