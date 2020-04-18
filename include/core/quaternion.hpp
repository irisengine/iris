#pragma once

#include <iosfwd>

#include "vector3.hpp"

namespace eng
{

/**
 * Class representing a Quaternion.
 *
 * A Quaternion represents a rotation (w) about a vector (x, y, z).
 */
class Quaternion
{
    public:

        /**
         * Construct a new unit Quaternion.
         */
        Quaternion();

        /**
         * Construct a Quaternion which represents a rotation about an axis.
         *
         * @param axis
         *   The axis about which to rotate.
         *
         * @param angle
         *   The rotation in radians.
         */
        Quaternion(const Vector3 &axis, const float angle);

        /**
         * Write a Quaternion to a stream, useful for debugging.
         *
         * @param out
         *   Stream to write to.
         *
         * @param q
         *   Quaternion to write to stream.
         *
         * @returns
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(
            std::ostream &out,
            const Quaternion &q);

        /**
         * Multiply this Quaternion by another, therefore applying a composition
         * of both rotations.
         *
         * @param quaternion
         *   Quaternion to compose with this.
         *
         * @returns
         *   Reference to this Quaternion.
         */
        Quaternion& operator*=(const Quaternion &quaternion);

        /**
         * Create a new Quaternion which is the composition of this this
         * rotation with another.
         *
         * @param q
         *   Quaternion to compose with this.
         *
         * @returns
         *   Copy of this Quaternion composed with the supplied one.
         */
        Quaternion operator*(const Quaternion &quaternion) const;

        /**
         * Add a rotation specified as a Vector3 to this Quaternion.
         *
         * @param vector
         *   Vector to add.
         *
         * @returns
         *   Reference to this Quaternion.
         */
        Quaternion& operator+=(const Vector3 &vector);

        /**
         * Create a new Quaternion that is the composition of this rotation
         * and one specified as a vector3.
         *
         * @param vector
         *   Vector to add.
         *
         * @returns
         *   Copy of this Quaternion composed with the Vector3 rotation.
         */
        Quaternion operator+(const Vector3 &vector) const;

        /**
         * Normalise this Quaternion.
         *
         * @returns
         *   A reference to this Quaternion.
         */
        Quaternion& normalise();

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

