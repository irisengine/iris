#include "vector3.hpp"

#include <cmath>
#include <iostream>

namespace eng
{

vector3::vector3()
    : x(0.0f),
      y(0.0f),
      z(0.0f)
{ }

vector3::vector3(const float x, const float y, const float z)
    : x(x),
      y(y),
      z(z)
{ }

std::ostream& operator<<(std::ostream &out, const vector3 &v)
{
    out << "x: " << v.x << " "
        << "y: " << v.y << " "
        << "z: " << v.z;

    return out;
}

vector3& vector3::operator*=(const float s)
{
    x *= s;
    y *= s;
    z *= s;

    return *this;
}

vector3 vector3::operator*(const float s) const
{
    return vector3(*this) *= s;
}

vector3& vector3::operator+=(const vector3 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

vector3 vector3::operator+(const vector3 &v) const
{
    return vector3(*this) += v;
}

vector3& vector3::operator-=(const vector3 &v)
{
    *this += -v;
    return *this;
}

vector3 vector3::operator-(const vector3 &v) const
{
    return vector3(*this) -= v;
}

vector3& vector3::operator*=(const vector3 &v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this;
}

vector3 vector3::operator*(const vector3 &v) const
{
    return vector3{ *this } *= v;
}

vector3 vector3::operator-() const
{
    return vector3{ -x, -y, -z };
}

float vector3::dot(const vector3 &v) const
{
    return x * v.x + y * v.y + z * v.z;
}

vector3& vector3::cross(const vector3 &v)
{
    const auto i = (y * v.z) - (z * v.y);
    const auto j = (x * v.z) - (z * v.x);
    const auto k = (x * v.y) - (y * v.x);

    x = i;
    y = -j;
    z = k;

    return *this;
}

vector3& vector3::normalise()
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

float vector3::magnitude() const
{
    return std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f));
}

vector3 vector3::cross(const vector3 &v1, const vector3 &v2)
{
    return vector3(v1).cross(v2);
}

vector3 vector3::normalise(const vector3 &v)
{
    return vector3(v).normalise();
}

}

