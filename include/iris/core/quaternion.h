#pragma once

#include <cmath>
#include <iosfwd>

#include "core/utils.h"
#include "core/vector3.h"

namespace iris
{

/**
 * Class representing a Quaternion.
 *
 * A Quaternion represents a rotation (w) about a vector (x, y, z).
 *
 * This is a header only class to allow for constexpr methods.
 */
class Quaternion
{
  public:
    /**
     * Construct a new unit Quaternion.
     */
    constexpr Quaternion()
        : w(1.0f)
        , x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {
    }

    /**
     * Construct a Quaternion which represents a rotation about an axis.
     *
     * @param axis
     *   The axis about which to rotate.
     *
     * @param angle
     *   The rotation in radians.
     */
    Quaternion(const Vector3 &axis, float angle)
        : w(0.0f)
        , x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {
        const auto half_angle = angle / 2.0f;
        const auto sin_angle = std::sin(half_angle);

        w = std::cos(half_angle);
        x = sin_angle * axis.x;
        y = sin_angle * axis.y;
        z = sin_angle * axis.z;

        normalise();
    }

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
    constexpr Quaternion(float x, float y, float z, float w)
        : w(w)
        , x(x)
        , y(y)
        , z(z)
    {
    }

    /**
     * Construct a Quaternion with Euler angles.
     *
     * @param yaw
     *   Yaw angle in radians.
     *
     * @param pitch
     *   Pitch angle in radians.
     *
     * @param roll
     *   Roll angle in radians.
     */
    Quaternion(float yaw, float pitch, float roll)
        : Quaternion()
    {
        const auto cy = std::cos(yaw * 0.5f);
        const auto sy = std::sin(yaw * 0.5f);
        const auto cp = std::cos(pitch * 0.5f);
        const auto sp = std::sin(pitch * 0.5f);
        const auto cr = std::cos(roll * 0.5f);
        const auto sr = std::sin(roll * 0.5f);

        w = cr * cp * cy + sr * sp * sy;
        x = sr * cp * cy - cr * sp * sy;
        y = cr * sp * cy + sr * cp * sy;
        z = cr * cp * sy - sr * sp * cy;
    }

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
    constexpr Quaternion &operator*=(const Quaternion &quaternion)
    {
        const Quaternion copy{*this};

        w = copy.w * quaternion.w - copy.x * quaternion.x -
            copy.y * quaternion.y - copy.z * quaternion.z;
        x = copy.w * quaternion.x + copy.x * quaternion.w +
            copy.y * quaternion.z - copy.z * quaternion.y;
        y = copy.w * quaternion.y + copy.y * quaternion.w +
            copy.z * quaternion.x - copy.x * quaternion.z;
        z = copy.w * quaternion.z + copy.z * quaternion.w +
            copy.x * quaternion.y - copy.y * quaternion.x;

        return *this;
    }

    /**
     * Create a new Quaternion which is the composition of this this
     * rotation with another.
     *
     * @param quaternion
     *   Quaternion to compose with this.
     *
     * @returns
     *   Copy of this Quaternion composed with the supplied one.
     */
    constexpr Quaternion operator*(const Quaternion &quaternion) const
    {
        return Quaternion{*this} *= quaternion;
    }

    /**
     * Add a rotation specified as a Vector3 to this Quaternion.
     *
     * @param vector
     *   Vector to add.
     *
     * @returns
     *   Reference to this Quaternion.
     */
    constexpr Quaternion &operator+=(const Vector3 &vector)
    {
        Quaternion q{};
        q.w = 0.0f;
        q.x = vector.x;
        q.y = vector.y;
        q.z = vector.z;

        q *= *this;

        w += q.w / 2.0f;
        x += q.x / 2.0f;
        y += q.y / 2.0f;
        z += q.z / 2.0f;

        return *this;
    }

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
    constexpr Quaternion operator+(const Vector3 &vector) const
    {
        return Quaternion{*this} += vector;
    }

    /**
     * Create a new Quaternion that is this value scaled.
     *
     * @param scale
     *   Amount to scale by.
     *
     * @returns
     *   Scaled quaternion.
     */
    constexpr Quaternion operator*(float scale) const
    {
        return Quaternion{*this} *= scale;
    }

    /**
     * Scale quaternion.
     *
     * @param scale
     *   Amount to scale by.
     *
     * @returns
     *   Reference to this Quaternion.
     */
    constexpr Quaternion &operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;
        w *= scale;

        return *this;
    }

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
    constexpr Quaternion operator-(const Quaternion &other) const
    {
        return Quaternion{*this} -= other;
    }

    /**
     * Subtract a Quaternion from this.
     *
     * @param other
     *   Quaternion to subtract from this.
     *
     * @returns
     *   Reference to this Quaternion.
     */
    constexpr Quaternion &operator-=(const Quaternion &other)
    {
        return *this += -other;
    }

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
    constexpr Quaternion operator+(const Quaternion &other) const
    {
        return Quaternion{*this} += other;
    }

    /**
     * Add a Quaternion from this.
     *
     * @param other
     *   Quaternion to subtract from this.
     *
     * @returns
     *   Reference to this Quaternion.
     */
    constexpr Quaternion &operator+=(const Quaternion &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;

        return *this;
    }

    /**
     * Negate operator.
     *
     * @returns
     *   Copy of this Quaternion with each component negated.
     */
    constexpr Quaternion operator-() const
    {
        return {-x, -y, -z, -w};
    }

    /**
     * Calculate the Quaternion dot product.
     *
     * @param other
     *   Quaternion to calculate dot product with.
     *
     * @returns
     *   Dot product of this Quaternion with the supplied one.
     */
    constexpr float dot(const Quaternion &other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    /**
     * Perform spherical linear interpolation toward target Quaternion.
     *
     * @param target
     *   Quaternion to interpolate towards.
     *
     * @param amount
     *   Amount to interpolate, must be in range [0.0, 1.0].
     */
    constexpr void slerp(Quaternion target, float amount)
    {
        auto dot = this->dot(target);
        if (dot < 0.0f)
        {
            target = -target;
            dot = -dot;
        }

        const auto threshold = 0.9995f;
        if (dot > threshold)
        {
            *this = *this + ((target - *this) * amount);
            normalise();
        }
        else
        {
            const auto theta_0 = std::acos(dot);
            const auto theta = theta_0 * amount;
            const auto sin_theta = std::sin(theta);
            const auto sin_theta_0 = std::sin(theta_0);

            const auto s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
            const auto s1 = sin_theta / sin_theta_0;

            *this = (*this * s0) + (target * s1);
        }
    }

    /**
     * Equality operator.
     *
     * @param other
     *   Quaternion to check for equality.
     *
     * @returns
     *   True if both Quaternion objects are the same, false otherwise.
     */
    bool operator==(const Quaternion &other) const
    {
        return compare(w, other.w) && compare(x, other.x) &&
               compare(y, other.y) && compare(z, other.z);
    }

    /**
     * Inequality operator.
     *
     * @param other
     *   Quaternion to check for inequality.
     *
     * @returns
     *   True if both Quaternion objects are not the same, false otherwise.
     */
    bool operator!=(const Quaternion &other) const
    {
        return !(*this == other);
    }

    /**
     * Normalise this Quaternion.
     *
     * @returns
     *   A reference to this Quaternion.
     */
    Quaternion &normalise()
    {
        const auto magnitude = std::pow(w, 2.0f) + std::pow(x, 2.0f) +
                               std::pow(y, 2.0f) + std::pow(z, 2.0f);

        if (magnitude == 0.0f)
        {
            w = 1.0f;
        }
        else
        {
            const auto d = std::sqrt(magnitude);

            w /= d;
            x /= d;
            y /= d;
            z /= d;
        }

        return *this;
    }

    /** Angle of rotation. */
    float w;

    /** x axis of rotation. */
    float x;

    /** y axis of rotation. */
    float y;

    /** z axis of rotation. */
    float z;
};

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
inline std::ostream &operator<<(std::ostream &out, const Quaternion &q)
{
    out << "x: " << q.x << " "
        << "y: " << q.y << " "
        << "z: " << q.z << " "
        << "w: " << q.w;

    return out;
}

}
