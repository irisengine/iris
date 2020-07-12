#include "core/quaternion.h"

#include <cmath>
#include <ostream>

#include "core/real.h"
#include "core/vector3.h"

namespace eng
{

Quaternion::Quaternion()
    : w(1.0f),
      x(0.0f),
      y(0.0f),
      z(0.0f)
{ }

Quaternion::Quaternion(const Vector3 &axis, const real angle)
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

Quaternion::Quaternion(real x, real y, real z, real w)
    : w(w),
      x(x),
      y(y),
      z(z)
{ }

std::ostream& operator<<(std::ostream &out, const Quaternion &q)
{
    out << "x: " << q.x << " "
        << "y: " << q.y << " "
        << "z: " << q.z << " "
        << "w: " << q.w;

    return out;
}

Quaternion& Quaternion::operator*=(const Quaternion &quaternion)
{
    const Quaternion copy{ *this };

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

Quaternion Quaternion::operator*(const Quaternion &quaternion) const
{
    return Quaternion{ *this } *= quaternion;
}

Quaternion& Quaternion::operator+=(const Vector3 &vector)
{
    Quaternion q{ };
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
    return Quaternion{ *this } += vector;
}

bool Quaternion::operator==(const Quaternion &other) const
{
    return (w == other.w) &&
           (x == other.x) &&
           (y == other.y) &&
           (z == other.z);
}

bool Quaternion::operator!=(const Quaternion &other) const
{
    return !(*this == other);
}

Quaternion& Quaternion::normalise()
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

