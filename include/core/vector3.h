#pragma once

#include <iostream>

namespace eng
{

/**
 * Class representing a vector in 3D space. Comprises an x, y and z
 * component.
 */
class Vector3
{
    public:

        /**
         * Constructs a new Vector3 with all components initialised to 0.
         */
        Vector3();

        /**
         * Constructs a new Vector3 with the supplied components.
         *
         * @param x
         *   x component.
         *
         * @param y
         *   y component.
         *
         * @param z
         *   z component.
         */
        Vector3(const float x, const float y, const float z);

        /**
         * Write a Vector3 to a stream, useful for debugging.
         *
         * @param out
         *   Stream to write to.
         *
         * @param v
         *   Vector3 to write to stream.
         *
         * @return
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(
            std::ostream &out,
            const Vector3 &v);

        /**
         * Multiply each component by a scalar value.
         *
         * @param scale
         *   scalar value.
         *
         * @return
         *   Reference to this vector3.
         */
        Vector3& operator*=(const float scale);

        /**
         * Create a new Vector3 which is this Vector3 with each component
         * multiplied by a scalar value.
         *
         * @param scale
         *   scalar value.
         *
         * @return
         *   Copy of this Vector3 with each component multiplied by a
         *   scalar value.
         */
        Vector3 operator*(const float scale) const;

        /**
         * Component wise add a Vector3 to this vector3.
         *
         * @param vector
         *   The Vector3 to add to this.
         *
         * @return
         *   Reference to this vector3.
         */
        Vector3& operator+=(const Vector3 &vector);

        /**
         * Create a new Vector3 which is this Vector3 added with a supplied
         * vector3.
         *
         * @param vector
         *   Vector3 to add to this.
         *
         * @return
         *   Copy of this Vector3 with each component added to the
         *   components of the supplied vector3.
         */
        Vector3 operator+(const Vector3 &vector) const;

        /**
         * Component wise subtract a Vector3 to this vector3.
         *
         * @param v
         *   The Vector3 to subtract from this.
         *
         * @return
         *   Reference to this vector3.
         */
        Vector3& operator-=(const Vector3 &vector);

        /**
         * Create a new Vector3 which is this Vector3 subtracted with a
         * supplied vector3.
         *
         * @param v
         *   Vector3 to subtract from this.
         *
         * @return
         *   Copy of this Vector3 with each component subtracted to the
         *   components of the supplied vector3.
         */
        Vector3 operator-(const Vector3 &vector) const;

        /**
         * Component wise multiple a Vector3 to this vector3.
         *
         * @param vector
         *   The Vector3 to multiply.
         *
         * @returns
         *   Reference to this vector3.
         */
        Vector3& operator*=(const Vector3 &vector);

        /**
         * Create a new Vector3 which us this Vector3 component wise multiplied
         * with a supplied vector3.
         *
         * @param vector
         *   Vector3 to multiply with this.
         *
         * @returns
         *   Copy of this Vector3 component wise multiplied with the supplied
         *   vector3.
         */
        Vector3 operator*(const Vector3 &vector) const;

        /**
         * Negate operator.
         *
         * @return
         *   Return a copy of this Vector3 with each component negated.
         */
        Vector3 operator-() const;

        /**
         * Calculate the vector dot product.
         *
         * @param vector
         *   Vector3 to calculate dot product with.
         *
         * @returns
         *   Dot product of this vector with supplied one.
         */
        float dot(const Vector3 &vector) const;

        /**
         * Perform cross product of this Vector3 with a supplied one.
         *
         * @param vector
         *   Vector3 to cross with.
         *
         * @return
         *   Reference to this vector3.
         */
        Vector3& cross(const Vector3 &vector);

        /**
         * Normalises this vector3.
         *
         * @return
         *   Reference to this vector3.
         */
        Vector3& normalise();

        /**
         * Get the magnitude of this vector.
         *
         * @retunrs
         *   Vector magnitude.
         */
        float magnitude() const;

        /**
         * Cross two Vector3 objects with each other.
         *
         * @param v1
         *   First Vector3 to cross.
         *
         * @param v2
         *   Second Vector3 to cross.
         *
         * @return
         *   v1 cross v2.
         */
        static Vector3 cross(const Vector3 &v1, const Vector3 &v2);

        /**
         * Normalise a supplied vector3.
         *
         * @param vector
         *   Vector3 to normalise.
         *
         * @return
         *   Supplied Vector3 normalised.
         */
        static Vector3 normalise(const Vector3 &vector);

        /** x component */
        float x;

        /** y component */
        float y;

        /** z component */
        float z;
};

}

