#pragma once

#include <iostream>

namespace eng
{

/**
 * Class representing a vector in 3D space. Comprises an x, y and z
 * component.
 */
class vector3
{
    public:

        /**
         * Constructs a new vector3 with all components initialised to 0.
         */
        vector3();

        /**
         * Constructs a new vector3 with the supplied components.
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
        vector3(const float x, const float y, const float z);

        /**
         * Write a vector3 to a stream, useful for debugging.
         *
         * @param out
         *   Stream to write to.
         *
         * @param v
         *   vector3 to write to stream.
         *
         * @return
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(
            std::ostream &out,
            const vector3 &v);

        /**
         * Multiply each component by a scalar value.
         *
         * @param s
         *   scalar value.
         *
         * @return
         *   Reference to this vector3.
         */
        vector3& operator*=(const float s);

        /**
         * Create a new vector3 which is this vector3 with each component
         * multiplied by a scalar value.
         *
         * @param s
         *   scalar value.
         *
         * @return
         *   Copy of this vector3 with each component multiplied by a
         *   scalar value.
         */
        vector3 operator*(const float s) const;

        /**
         * Component wise add a vector3 to this vector3.
         *
         * @param v
         *   The vector3 to add to this.
         *
         * @return
         *   Reference to this vector3.
         */
        vector3& operator+=(const vector3 &v);

        /**
         * Create a new vector3 which is this vector3 added with a supplied
         * vector3.
         *
         * @param v
         *   vector3 to add to this.
         *
         * @return
         *   Copy of this vector3 with each component added to the
         *   components of the supplied vector3.
         */
        vector3 operator+(const vector3 &v) const;

        /**
         * Component wise subtract a vector3 to this vector3.
         *
         * @param v
         *   The vector3 to subtract from this.
         *
         * @return
         *   Reference to this vector3.
         */
        vector3& operator-=(const vector3 &v);

        /**
         * Create a new vector3 which is this vector3 subtracted with a
         * supplied vector3.
         *
         * @param v
         *   vector3 to subtract from this.
         *
         * @return
         *   Copy of this vector3 with each component subtracted to the
         *   components of the supplied vector3.
         */
        vector3 operator-(const vector3 &v) const;

        /**
         * Component wise multiple a vector3 to this vector3.
         *
         * @param v
         *   The vector3 to multiply.
         *
         * @returns
         *   Reference to this vector3.
         */
        vector3& operator*=(const vector3 &v);

        /**
         * Create a new vector3 which us this vector3 component wise multiplied
         * with a supplied vector3.
         *
         * @param c
         *   vector3 to multiply with this.
         *
         * @returns
         *   Copy of this vector3 component wise multiplied with the supplied
         *   vector3.
         */
        vector3 operator*(const vector3 &v) const;

        /**
         * Negate operator.
         *
         * @return
         *   Return a copy of this vector3 with each component negated.
         */
        vector3 operator-() const;

        /**
         * Calculate the vector dot product.
         *
         * @param v
         *   vector3 to calculate dot product with.
         *
         * @returns
         *   Dot product of this vector with supplied one.
         */
        float dot(const vector3 &v) const;

        /**
         * Perform cross product of this vector3 with a supplied one.
         *
         * @param v
         *   vector3 to cross with.
         *
         * @return
         *   Reference to this vector3.
         */
        vector3& cross(const vector3 &v);

        /**
         * Normalises this vector3.
         *
         * @return
         *   Reference to this vector3.
         */
        vector3& normalise();

        /**
         * Get the magnitude of this vector.
         *
         * @retunrs
         *   Vector magnitude.
         */
        float magnitude() const;

        /**
         * Cross two vector3 objects with each other.
         *
         * @param v1
         *   First vector3 to cross.
         *
         * @param v2
         *   Second vector3 to cross.
         *
         * @return
         *   v1 cross v2.
         */
        static vector3 cross(const vector3 &v1, const vector3 &v2);

        /**
         * Normalise a supplied vector3.
         *
         * @param v
         *   vector3 to normalise.
         *
         * @return
         *   Supplied vector3 normalised.
         */
        static vector3 normalise(const vector3 &v);

        /** x component */
        float x;

        /** y component */
        float y;

        /** z component */
        float z;
};

}

