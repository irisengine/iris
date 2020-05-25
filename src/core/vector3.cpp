#include "core/vector3.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>

#include "core/real.h"

namespace eng
{

Vector3::Vector3()
    : x(0.0f),
      y(0.0f),
      z(0.0f)
{ }

Vector3::Vector3(real x, real y, real z)
    : x(x),
      y(y),
      z(z)
{ }

std::ostream& operator<<(std::ostream &out, const Vector3 &v)
{
    out << std::setprecision(std::numeric_limits<real>::digits10 + 1);
    out << "x: " << v.x << " "
        << "y: " << v.y << " "
        << "z: " << v.z;

    return out;
}

Vector3& Vector3::operator*=(real scale)
{
    x *= scale;
    y *= scale;
    z *= scale;

    return *this;
}

Vector3 Vector3::operator*(real scale) const
{
    return Vector3(*this) *= scale;
}

Vector3& Vector3::operator+=(const Vector3 &vector)
{
    x += vector.x;
    y += vector.y;
    z += vector.z;

    return *this;
}

Vector3 Vector3::operator+(const Vector3 &vector) const
{
    return Vector3(*this) += vector;
}

Vector3& Vector3::operator-=(const Vector3 &vector)
{
    *this += -vector;
    return *this;
}

Vector3 Vector3::operator-(const Vector3 &vector) const
{
    return Vector3(*this) -= vector;
}

Vector3& Vector3::operator*=(const Vector3 &vector)
{
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;

    return *this;
}

Vector3 Vector3::operator*(const Vector3 &vector) const
{
    return Vector3{ *this } *= vector;
}

Vector3 Vector3::operator-() const
{
    return Vector3{ -x, -y, -z };
}

bool Vector3::operator==(const Vector3 &other) const
{
    return (x == other.x) &&
           (y == other.y) &&
           (z == other.z);
}

bool Vector3::operator!=(const Vector3 &other) const
{
    return !(other == *this);
}

real Vector3::dot(const Vector3 &vector) const
{
    return x * vector.x + y * vector.y + z * vector.z;
}

Vector3& Vector3::cross(const Vector3 &vector)
{
    const auto i = (y * vector.z) - (z * vector.y);
    const auto j = (x * vector.z) - (z * vector.x);
    const auto k = (x * vector.y) - (y * vector.x);

    x = i;
    y = -j;
    z = k;

    return *this;
}

Vector3& Vector3::normalise()
{
    const auto length =
        std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f));

    if(length != 0.0f)
    {
        x /= length;
        y /= length;
        z /= length;
    }

    return *this;
}

real Vector3::magnitude() const
{
    return std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f));
}

Vector3 Vector3::cross(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1).cross(v2);
}

Vector3 Vector3::normalise(const Vector3 &vector)
{
    return Vector3(vector).normalise();
}

}

