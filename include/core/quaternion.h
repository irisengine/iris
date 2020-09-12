#pragma once

#include <iosfwd>

#include "core/real.h"
#include "core/vector3.h"

namespace iris
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
        Quaternion(const Vector3 &axis, const real angle);

        /**
         * Construct a Quaternion with supplied components.
         *
         * @param x
         *   x component.
         *
         * @param y
         *   x component.
         *
         * @param z
         *   x component.
         *
         * @param w
         *   x component.
         */
        Quaternion(real x, real y, real z, real w);

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
         * Create a new Quaternion that is this value scaled.
         * 
         * @param scale
         *   Amount to scale by.
         * 
         * @returns
         *   Scaled quaternion.
         */
        Quaternion operator*(real scale) const;

        /**
         * Scale quaternion.
         * 
         * @param scale
         *   Amount to scale by.
         * 
         * @returns
         *   Reference to this Quaternion.
         */
        Quaternion& operator*=(real scale);

        /**
         * Create a new Quaternion that is this Quaternion subtracted with a 
         * supplied Quaternion.
         * 
         * @param other
         *   Quaternion to subtract from this.
         * 
         * @returns
         *   Copy of this Quaternion after subtraction.
         */
        Quaternion operator-(const Quaternion &other) const;

        /**
         * Subtract a Quaternion from this.
         * 
         * @param other
         *   Quaternion to subtract from this.
         * 
         * @returns
         *   Reference to this Quaternion.
         */
        Quaternion& operator-=(const Quaternion &other);

        /**
         * Create a new Quaternion that is this Quaternion added with a 
         * supplied Quaternion.
         * 
         * @param other
         *   Quaternion to add to this.
         * 
         * @returns
         *   Copy of this Quaternion after addition.
         */
        Quaternion operator+(const Quaternion &other) const;

        /**
         * Add a Quaternion from this.
         * 
         * @param other
         *   Quaternion to subtract from this.
         * 
         * @returns
         *   Reference to this Quaternion.
         */
        Quaternion& operator+=(const Quaternion &other);

        /**
         * Negate operator.
         * 
         * @returns
         *   Copy of this Quaternion with each component negated.
         */
        Quaternion operator-() const;

        /**
         * Calculate the Quaternion dot product.
         * 
         * @param other
         *   Quaternion to calculate dot product with.
         * 
         * @returns
         *   Dot product of this Quaternion with the supplied one.
         */
        real dot(const Quaternion &other) const;

        /**
         * Perform spherical linear interpolation toward target Quaternion.
         * 
         * @param target
         *   Quaternion to interpolate towards.
         * 
         * @param amount
         *   Amount to interpolate, must be in range [0.0, 1.0].
         */
        void slerp(Quaternion target, real amount);

        /**
         * Equality operator.
         *
         * @param other
         *   Quaternion to check for equality.
         *
         * @returns
         *   True if both Quaternion objects are the same, false otherwise.
         */
        bool operator==(const Quaternion &other) const;

        /**
         * Inequality operator.
         *
         * @param other
         *   Quaternion to check for inequality.
         *
         * @returns
         *   True if both Quaternion objects are not the same, false otherwise.
         */
        bool operator!=(const Quaternion &other) const;

        /**
         * Normalise this Quaternion.
         *
         * @returns
         *   A reference to this Quaternion.
         */
        Quaternion& normalise();

        /** Angle of rotation. */
        real w;

        /** x axis of rotation. */
        real x;

        /** y axis of rotation. */
        real y;

        /** z axis of rotation. */
        real z;
};

}

