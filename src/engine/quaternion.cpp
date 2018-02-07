#include "quaternion.hpp"

#include <cmath>
#include <iostream>

#include "vector3.hpp"

namespace eng
{

quaternion::quaternion()
    : w(1.0f),
      x(0.0f),
      y(0.0f),
      z(0.0f)
{ }

quaternion::quaternion(const vector3 &axis, const float angle)
    : w(0.0f),
      x(0.0f),
      y(0.0f),
      z(0.0f)
{
    const auto half_angle = angle / 2.0f;
    const auto sin_angle = std::sin(half_angle);

    w = std::cos(half_angle);
    x = sin_angle * axis.x;
    y = sin_angle * axis.y;
    z = sin_angle * axis.z;

    normalise();
}

std::ostream& operator<<(std::ostream &out, const quaternion &q) noexcept
{
    out << "w: " << q.w << " "
        << "x: " << q.x << " "
        << "y: " << q.y << " "
        << "z: " << q.z;

    return out;
}

quaternion& quaternion::operator*=(const quaternion &q)
{
    const quaternion copy{ *this };

    w = copy.w * q.w - copy.x * q.x -
        copy.y * q.y - copy.z * q.z;
    x = copy.w * q.x + copy.x * q.w +
        copy.y * q.z - copy.z * q.y;
    y = copy.w * q.y + copy.y * q.w +
        copy.z * q.x - copy.x * q.z;
    z = copy.w * q.z + copy.z * q.w +
        copy.x * q.y - copy.y * q.x;

    return *this;
}

quaternion quaternion::operator*(const quaternion &q) const
{
    return quaternion{ *this } *= q;
}

quaternion& quaternion::operator+=(const vector3 &v)
{
    quaternion q{ };
    q.w = 0.0f;
    q.x = v.x;
    q.y = v.y;
    q.z = v.z;

    q *= *this;

    w += q.w / 2.0f;
    x += q.x / 2.0f;
    y += q.y / 2.0f;
    z += q.z / 2.0f;

    return *this;
}

quaternion quaternion::operator+(const vector3 &v) const
{
    return quaternion{ *this } += v;
}

quaternion& quaternion::normalise() noexcept
{
    const auto magnitude =
        std::pow(w, 2.0f) +
        std::pow(x, 2.0f) +
        std::pow(y, 2.0f) +
        std::pow(z, 2.0f);

    if(magnitude == 0.0f)
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

