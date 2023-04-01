////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <ostream>

#include "core/utils.h"

namespace iris
{

/**
 * Class representing a vector in 3D space. Comprises an x, y and z
 * component.
 *
 * This is a header only class to allow for constexpr methods.
 */
class Vector3
{
  public:
    /**
     * Constructs a new Vector3 with all components initialised to 0.
     */
    constexpr Vector3()
        : Vector3(0.0f)
    {
    }

    /**
     * Constructs a new Vector3 with all components initialised to the same
     * value.
     *
     * @param xyz
     *   Value for x, y and z.
     */
    constexpr Vector3(float xyz)
        : Vector3(xyz, xyz, xyz)
    {
    }

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
    constexpr Vector3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    /**
     * Multiply each component by a scalar value.
     *
     * @param scale
     *   scalar value.
     *
     * @return
     *   Reference to this vector3.
     */
    constexpr Vector3 &operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;

        return *this;
    }

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
    constexpr Vector3 operator*(float scale) const
    {
        return Vector3(*this) *= scale;
    }

    /**
     * Divide each component by a scalar value.
     *
     * @param scale
     *   scalar value.
     *
     * @return
     *   Reference to this vector3.
     */
    constexpr Vector3 &operator/=(float scale)
    {
        x /= scale;
        y /= scale;
        z /= scale;

        return *this;
    }

    /**
     * Create a new Vector3 which is this Vector3 with each component divided by a scalar value.
     *
     * @param scale
     *   scalar value.
     *
     * @return
     *   Copy of this Vector3 with each component divided by a scalar value.
     */
    constexpr Vector3 operator/(float scale) const
    {
        return Vector3(*this) /= scale;
    }

    /**
     * Component wise add a Vector3 to this vector3.
     *
     * @param vector
     *   The Vector3 to add to this.
     *
     * @return
     *   Reference to this vector3.
     */
    constexpr Vector3 &operator+=(const Vector3 &vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;

        return *this;
    }

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
    constexpr Vector3 operator+(const Vector3 &vector) const
    {
        return Vector3(*this) += vector;
    }

    /**
     * Component wise subtract a Vector3 to this vector3.
     *
     * @param v
     *   The Vector3 to subtract from this.
     *
     * @return
     *   Reference to this vector3.
     */
    constexpr Vector3 &operator-=(const Vector3 &vector)
    {
        *this += -vector;
        return *this;
    }

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
    constexpr Vector3 operator-(const Vector3 &vector) const
    {
        return Vector3(*this) -= vector;
    }

    /**
     * Component wise multiple a Vector3 to this vector3.
     *
     * @param vector
     *   The Vector3 to multiply.
     *
     * @returns
     *   Reference to this vector3.
     */
    constexpr Vector3 &operator*=(const Vector3 &vector)
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;

        return *this;
    }

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
    constexpr Vector3 operator*(const Vector3 &vector) const
    {
        return Vector3{*this} *= vector;
    }

    /**
     * Negate operator.
     *
     * @return
     *   Return a copy of this Vector3 with each component negated.
     */
    constexpr Vector3 operator-() const
    {
        return Vector3{-x, -y, -z};
    }

    /**
     * Equality operator.
     *
     * @param other
     *   Vector3 to check for equality.
     *
     * @returns
     *   True if both Vector3 objects are the same, false otherwise.
     */
    bool operator==(const Vector3 &other) const
    {
        return compare(x, other.x) && compare(y, other.y) && compare(z, other.z);
    }

    /**
     * Inequality operator.
     *
     * @param other
     *   Vector3 to check for inequality.
     *
     * @returns
     *   True if both Vector3 objects are not the same, false otherwise.
     */
    bool operator!=(const Vector3 &other) const
    {
        return !(other == *this);
    }

    /**
     * Calculate the vector dot product.
     *
     * @param vector
     *   Vector3 to calculate dot product with.
     *
     * @returns
     *   Dot product of this vector with supplied one.
     */
    constexpr float dot(const Vector3 &vector) const
    {
        return x * vector.x + y * vector.y + z * vector.z;
    }

    /**
     * Perform cross product of this Vector3 with a supplied one.
     *
     * @param vector
     *   Vector3 to cross with.
     *
     * @return
     *   Reference to this vector3.
     */
    constexpr Vector3 &cross(const Vector3 &vector)
    {
        const auto i = (y * vector.z) - (z * vector.y);
        const auto j = (x * vector.z) - (z * vector.x);
        const auto k = (x * vector.y) - (y * vector.x);

        x = i;
        y = -j;
        z = k;

        return *this;
    }

    /**
     * Normalises this vector3.
     *
     * @return
     *   Reference to this vector3.
     */
    Vector3 &normalise()
    {
        const auto length =

            std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f));

        if (length != 0.0f)
        {
            x /= length;
            y /= length;
            z /= length;
        }

        return *this;
    }

    /**
     * Get the magnitude of this vector.
     *
     * @return
     *   Vector magnitude.
     */
    float magnitude() const
    {
        return std::hypot(x, y, z);
    }

    /**
     * Linear interpolate between this and another vector.
     *
     * @param other
     *   Vector3 to interpolate to.
     *
     * @param amount
     *   Interpolation amount, must be in range [0.0, 1.0].
     */
    constexpr void lerp(const Vector3 &other, float amount)
    {
        *this *= (1.0f - amount);
        *this += (other * amount);
    }

    /**
     * Linear interpolate between two vectors.
     *
     * @param start
     *   Vector3 to start from.
     *
     * @param end
     *   Vector3 to lerp towards.
     *
     * @param amount
     *   Interpolation amount, must be in range [0.0, 1.0].
     *
     * @returns
     *   Result of lerp.
     */
    constexpr static Vector3 lerp(const Vector3 &start, const Vector3 &end, float amount)
    {
        auto tmp = start;
        tmp.lerp(end, amount);

        return tmp;
    }

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
    constexpr static Vector3 cross(const Vector3 &v1, const Vector3 &v2)
    {
        return Vector3(v1).cross(v2);
    }

    /**
     * Normalise a supplied vector3.
     *
     * @param vector
     *   Vector3 to normalise.
     *
     * @return
     *   Supplied Vector3 normalised.
     */
    static Vector3 normalise(const Vector3 &vector)
    {
        return Vector3(vector).normalise();
    }

    /**
     * Get the Euclidean distance between two vectors.
     *
     * @param a
     *   Point to calculate distance from.
     *
     * @param b
     *   Point to calculate distance to.
     *
     * @returns
     *   Distance between point a and b.
     */
    static float distance(const Vector3 &a, const Vector3 &b)
    {
        return (b - a).magnitude();
    }

    /** x component */
    float x;

    /** y component */
    float y;

    /** z component */
    float z;
};

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
inline std::ostream &operator<<(std::ostream &out, const Vector3 &v)
{
    out << "x: " << v.x << " "
        << "y: " << v.y << " "
        << "z: " << v.z;

    return out;
}

}

// specialise std::hash for colour
namespace std
{

template <>
struct hash<iris::Vector3>
{
    size_t operator()(const iris::Vector3 &vec) const
    {
        return iris::combine_hash(vec.x, vec.y, vec.z);
    }
};

}
