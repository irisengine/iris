#include "core/vector3.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>

#include "core/utils.h"

namespace iris
{

Vector3::Vector3()
    : Vector3(0.0f)
{
}

Vector3::Vector3(float xyz)
    : Vector3(xyz, xyz, xyz)
{
}

Vector3::Vector3(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{
}

std::ostream &operator<<(std::ostream &out, const Vector3 &v)
{
    out << "x: " << v.x << " "
        << "y: " << v.y << " "
        << "z: " << v.z;

    return out;
}

Vector3 &Vector3::operator*=(float scale)
{
    x *= scale;
    y *= scale;
    z *= scale;

    return *this;
}

Vector3 Vector3::operator*(float scale) const
{
    return Vector3(*this) *= scale;
}

Vector3 &Vector3::operator+=(const Vector3 &vector)
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

Vector3 &Vector3::operator-=(const Vector3 &vector)
{
    *this += -vector;
    return *this;
}

Vector3 Vector3::operator-(const Vector3 &vector) const
{
    return Vector3(*this) -= vector;
}

Vector3 &Vector3::operator*=(const Vector3 &vector)
{
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;

    return *this;
}

Vector3 Vector3::operator*(const Vector3 &vector) const
{
    return Vector3{*this} *= vector;
}

Vector3 Vector3::operator-() const
{
    return Vector3{-x, -y, -z};
}

bool Vector3::operator==(const Vector3 &other) const
{
    return compare(x, other.x) && compare(y, other.y) && compare(z, other.z);
}

bool Vector3::operator!=(const Vector3 &other) const
{
    return !(other == *this);
}

float Vector3::dot(const Vector3 &vector) const
{
    return x * vector.x + y * vector.y + z * vector.z;
}

Vector3 &Vector3::cross(const Vector3 &vector)
{
    const auto i = (y * vector.z) - (z * vector.y);
    const auto j = (x * vector.z) - (z * vector.x);
    const auto k = (x * vector.y) - (y * vector.x);

    x = i;
    y = -j;
    z = k;

    return *this;
}

Vector3 &Vector3::normalise()
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

float Vector3::magnitude() const
{
    return std::hypot(x, y, z);
}

void Vector3::lerp(const Vector3 &other, float amount)
{
    *this *= (1.0f - amount);
    *this += (other * amount);
}

Vector3 Vector3::cross(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1).cross(v2);
}

Vector3 Vector3::normalise(const Vector3 &vector)
{
    return Vector3(vector).normalise();
}

float Vector3::distance(const Vector3 &a, const Vector3 &b)
{
    return (b - a).magnitude();
}

}
