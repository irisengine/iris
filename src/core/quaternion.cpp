#include "core/quaternion.h"

#include <cmath>
#include <ostream>

#include "core/utils.h"
#include "core/vector3.h"

namespace iris
{

Quaternion::Quaternion()
    : w(1.0f)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f)
{
}

Quaternion::Quaternion(const Vector3 &axis, float angle)
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

Quaternion::Quaternion(float x, float y, float z, float w)
    : w(w)
    , x(x)
    , y(y)
    , z(z)
{
}

Quaternion::Quaternion(float yaw, float pitch, float roll)
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

std::ostream &operator<<(std::ostream &out, const Quaternion &q)
{
    out << "x: " << q.x << " "
        << "y: " << q.y << " "
        << "z: " << q.z << " "
        << "w: " << q.w;

    return out;
}

Quaternion &Quaternion::operator*=(const Quaternion &quaternion)
{
    const Quaternion copy{*this};

    w = copy.w * quaternion.w - copy.x * quaternion.x - copy.y * quaternion.y -
        copy.z * quaternion.z;
    x = copy.w * quaternion.x + copy.x * quaternion.w + copy.y * quaternion.z -
        copy.z * quaternion.y;
    y = copy.w * quaternion.y + copy.y * quaternion.w + copy.z * quaternion.x -
        copy.x * quaternion.z;
    z = copy.w * quaternion.z + copy.z * quaternion.w + copy.x * quaternion.y -
        copy.y * quaternion.x;

    return *this;
}

Quaternion Quaternion::operator*(const Quaternion &quaternion) const
{
    return Quaternion{*this} *= quaternion;
}

Quaternion &Quaternion::operator+=(const Vector3 &vector)
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

Quaternion Quaternion::operator+(const Vector3 &vector) const
{
    return Quaternion{*this} += vector;
}

Quaternion Quaternion::operator*(float scale) const
{
    return Quaternion{*this} *= scale;
}

Quaternion &Quaternion::operator*=(float scale)
{
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;

    return *this;
}

Quaternion Quaternion::operator-(const Quaternion &other) const
{
    return Quaternion{*this} -= other;
}

Quaternion &Quaternion::operator-=(const Quaternion &other)
{
    return *this += -other;
}

Quaternion Quaternion::operator+(const Quaternion &other) const
{
    return Quaternion{*this} += other;
}

Quaternion &Quaternion::operator+=(const Quaternion &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Quaternion Quaternion::operator-() const
{
    return {-x, -y, -z, -w};
}

float Quaternion::dot(const Quaternion &other) const
{
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

void Quaternion::slerp(Quaternion target, float amount)
{
    auto dot = this->dot(target);
    if (dot < 0.0f)
    {
        target = -target;
        dot = -dot;
    }

    static const auto threshold = 0.9995f;
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

bool Quaternion::operator==(const Quaternion &other) const
{
    return compare(w, other.w) && compare(x, other.x) && compare(y, other.y) &&
           compare(z, other.z);
}

bool Quaternion::operator!=(const Quaternion &other) const
{
    return !(*this == other);
}

Quaternion &Quaternion::normalise()
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

}
