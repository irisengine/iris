#include "vector3.hpp"

#include <cmath>
#include <iostream>

namespace eng
{

vector3::vector3() noexcept
    : x(0.0f),
      y(0.0f),
      z(0.0f)
{ }

vector3::vector3(const float x, const float y, const float z) noexcept
    : x(x),
      y(y),
      z(z)
{ }

std::ostream& operator<<(std::ostream &out, const vector3 &v) noexcept
{
    out << "x: " << v.x << " "
        << "y: " << v.y << " "
        << "z: " << v.z;

    return out;
}

vector3& vector3::operator*=(const float s) noexcept
{
    x *= s;
    y *= s;
    z *= s;

    return *this;
}

vector3 vector3::operator*(const float s) const noexcept
{
    return vector3(*this) *= s;
}

vector3& vector3::operator+=(const vector3 &v) noexcept
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

vector3 vector3::operator+(const vector3 &v) const noexcept
{
    return vector3(*this) += v;
}

vector3& vector3::operator-=(const vector3 &v) noexcept
{
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

vector3 vector3::operator-(const vector3 &v) const noexcept
{
    return vector3(*this) -= v;
}

vector3 vector3::operator-() const noexcept
{
    return vector3(-x, -y, -z);

float vector3::dot(const vector3 &v) const noexcept
{
    return x * v.x + y * v.y + z * v.z;
}

vector3& vector3::cross(const vector3 &v) noexcept
{
    const auto i = (y * v.z) - (z * v.y);
    const auto j = (x * v.z) - (z * v.x);
    const auto k = (x * v.y) - (y * v.x);

    x = i;
    y = -j;
    z = k;

    return *this;
}

vector3& vector3::normalise() noexcept
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

float vector3::magnitude() const noexcept
{
    return std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f));
}

vector3 vector3::cross(const vector3 &v1, const vector3 &v2) noexcept
{
    return vector3(v1).cross(v2);
}

vector3 vector3::normalise(const vector3 &v) noexcept
{
    return vector3(v).normalise();
}

}

