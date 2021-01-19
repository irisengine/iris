#include "core/colour.h"

#include <cstddef>
#include <cstdint>
#include <ostream>

#include "core/utils.h"

namespace
{

float byte_to_float(std::int32_t value)
{
    return static_cast<float>(value) / 255.0f;
}

}

namespace iris
{

Colour::Colour()
    : Colour(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Colour::Colour(float r, float g, float b, float a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}

Colour::Colour(std::int32_t r, std::int32_t g, std::int32_t b, std::int32_t a)
    : Colour(
          byte_to_float(r),
          byte_to_float(g),
          byte_to_float(b),
          byte_to_float(a))
{
}

std::ostream &operator<<(std::ostream &out, const Colour &c)
{
    out << "r: " << c.r << " "
        << "g: " << c.g << " "
        << "b: " << c.b << " "
        << "a: " << c.a;

    return out;
}

Colour &Colour::operator*=(float scale)
{
    r *= scale;
    g *= scale;
    b *= scale;
    a *= scale;

    return *this;
}

Colour Colour::operator*(float scale) const
{
    return Colour(*this) *= scale;
}

Colour &Colour::operator+=(const Colour &colour)
{
    r += colour.r;
    g += colour.g;
    b += colour.b;
    a += colour.a;

    return *this;
}

Colour Colour::operator+(const Colour &colour) const
{
    return Colour(*this) += colour;
}

Colour &Colour::operator-=(const Colour &colour)
{
    r -= colour.r;
    g -= colour.g;
    b -= colour.b;
    a -= colour.a;

    return *this;
}

Colour Colour::operator-(const Colour &colour) const
{
    return Colour(*this) -= colour;
}

Colour &Colour::operator*=(const Colour &colour)
{
    r *= colour.r;
    g *= colour.g;
    b *= colour.b;
    a *= colour.a;

    return *this;
}

Colour Colour::operator*(const Colour &colour) const
{
    return Colour{*this} *= colour;
}

bool Colour::operator==(const Colour &other) const
{
    return compare(r, other.r) && compare(g, other.g) && compare(b, other.b) &&
           compare(a, other.a);
}

bool Colour::operator!=(const Colour &other) const
{
    return !(other == *this);
}

}
