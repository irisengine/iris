#pragma once

#include <iosfwd>

#include "vector3.hpp"

namespace eng
{

/**
 * Class representing a quaternion.
 *
 * A quaternion represents a rotation (w) about a vector (x, y, z).
 */
class quaternion final
{
    public:

        /**
         * Construct a new unit quaternion.
         */
        quaternion();

        /**
         * Construct a quaternion which represents a rotation about an axis.
         *
         * @param axis
         *   The axis about which to rotate.
         *
         * @param angle
         *   The rotation in radians.
         */
        quaternion(const vector3 &axis, const float angle);

        /** Default */
        ~quaternion() = default;
        quaternion(const quaternion &) = default;
        quaternion& operator=(const quaternion&) = default;
        quaternion(quaternion&&) = default;
        quaternion& operator=(quaternion&&) = default;

        /**
         * Write a quaternion to a stream, useful for debugging.
         *
         * @param out
         *   Stream to write to.
         *
         * @param q
         *   quaternion to write to stream.
         *
         * @returns
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(
            std::ostream &out,
            const quaternion &q) noexcept;

        /**
         * Multiply this quaternion by another, therefore applying a composition
         * of both rotations.
         *
         * @param q
         *   Quaternion to compose with this.
         *
         * @returns
         *   Reference to this quaternion.
         */
        quaternion& operator*=(const quaternion &q);

        /**
         * Create a new quaternion which is the composition of this this
         * rotation with another.
         *
         * @param q
         *   Quaternion to compose with this.
         *
         * @returns
         *   Copy of this quaternion composed with the supplied one.
         */
        quaternion operator*(const quaternion &q) const;

        /**
         * Add a rotation specified as a vector3 to this quaternion.
         *
         * @param v
         *   Vector to add.
         *
         * @returns
         *   Reference to this quaternion.
         */
        quaternion& operator+=(const vector3 &v);

        /**
         * Create a new quaternion that is the composition of this rotation
         * and one specified as a vector3.
         *
         * @param v
         *   Vector to add.
         *
         * @returns
         *   Copy of this quaternion composed with the vector3 rotation.
         */
        quaternion operator+(const vector3 &v) const;

        /**
         * Normalise this quaternion.
         *
         * @returns
         *   A reference to this quaternion.
         */
        quaternion& normalise() noexcept;

        /** Angle of rotation. */
        float w;

        /** x axis of rotation. */
        float x;

        /** y axis of rotation. */
        float y;

        /** z axis of rotation. */
        float z;
};

}

