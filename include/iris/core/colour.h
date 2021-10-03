#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>

#include "core/utils.h"

namespace iris
{
/**
 * A colour with red, green, blue and alpha components.
 */
class Colour
{
  public:
    /**
     * Create a black colour.
     */
    constexpr Colour()
        : Colour(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    /**
     * Create a new colour with float components.
     *
     * @param r
     *   Red.
     *
     * @param g
     *   Green.
     *
     * @param b
     *   Blue.
     *
     * @param a
     *   Alpha.
     */
    constexpr Colour(float r, float g, float b, float a = 1.0f)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }

    /**
     * Create a new colour with integer components.
     *
     * Ideally this would take the values as std::uint8_t, but this causes an
     * ambiguity when integers are passes with the float constructor.
     *
     * @param r
     *   Red.
     *
     * @param g
     *   Green.
     *
     * @param b
     *   Blue.
     *
     * @param a
     *   Alpha.
     */
    constexpr Colour(
        std::int32_t r,
        std::int32_t g,
        std::int32_t b,
        std::int32_t a = 255)
        : Colour(
              static_cast<float>(r) / 255.0f,
              static_cast<float>(g) / 255.0f,
              static_cast<float>(b) / 255.0f,
              static_cast<float>(a) / 255.0f)
    {
    }

    /**
     * Multiply each component by a scalar value.
     *
     * @param scale
     *   Scalar value.
     *
     * @return
     *   Reference to this Colour.
     */
    constexpr Colour &operator*=(float scale)
    {
        r *= scale;
        g *= scale;
        b *= scale;
        a *= scale;

        return *this;
    }

    /**
     * Create a new Colour which is this Colour with each component
     * multiplied by a scalar value.
     *
     * @param scale
     *   scalar value.
     *
     * @return
     *   Copy of this Colour with each component multiplied by a
     *   scalar value.
     */
    constexpr Colour operator*(float scale) const
    {
        return Colour(*this) *= scale;
    }

    /**
     * Component wise add a Colour to this Colour.
     *
     * @param colour
     *   The Colour to add to this.
     *
     * @return
     *   Reference to this colour.
     */
    constexpr Colour &operator+=(const Colour &colour)
    {
        r += colour.r;
        g += colour.g;
        b += colour.b;
        a += colour.a;

        return *this;
    }

    /**
     * Create a new Colour which is this Colour added with a supplied
     * Colour.
     *
     * @param colour
     *   Colour to add to this.
     *
     * @return
     *   Copy of this Colour with each component added to the
     *   components of the supplied Colour.
     */
    constexpr Colour operator+(const Colour &colour) const
    {
        return Colour(*this) += colour;
    }

    /**
     * Component wise subtract a Colour to this Colour.
     *
     * @param v
     *   The Colour to subtract from this.
     *
     * @return
     *   Reference to this Colour.
     */
    constexpr Colour &operator-=(const Colour &colour)
    {
        r -= colour.r;
        g -= colour.g;
        b -= colour.b;
        a -= colour.a;

        return *this;
    }

    /**
     * Create a new Colour which is this Colour subtracted with a
     * supplied Colour.
     *
     * @param colour
     *   Colour to subtract from this.
     *
     * @return
     *   Copy of this Colour with each component subtracted to the
     *   components of the supplied Colour.
     */
    constexpr Colour operator-(const Colour &colour) const
    {
        return Colour(*this) -= colour;
    }

    /**
     * Component wise multiple a Colour to this Colour.
     *
     * @param colour
     *   The Colour to multiply.
     *
     * @returns
     *   Reference to this Colour.
     */
    constexpr Colour &operator*=(const Colour &colour)
    {
        r *= colour.r;
        g *= colour.g;
        b *= colour.b;
        a *= colour.a;

        return *this;
    }

    /**
     * Create a new Colour which us this Colour component wise multiplied
     * with a supplied Colour.
     *
     * @param colour
     *   Colour to multiply with this.
     *
     * @returns
     *   Copy of this Colour component wise multiplied with the supplied
     *   Colour.
     */
    constexpr Colour operator*(const Colour &colour) const
    {
        return Colour{*this} *= colour;
    }

    /**
     * Check if this colour is equal to another.
     *
     * @param other
     *   Colour to compare with.
     *
     * @returns
     *   True if both Colours are equal, otherwise false.
     */
    bool operator==(const Colour &other) const
    {
        return compare(r, other.r) && compare(g, other.g) &&
               compare(b, other.b) && compare(a, other.a);
    }

    /**
     * Check if this colour is not equal to another.
     *
     * @param other
     *   Colour to compare with.
     *
     * @returns
     *   True if both Colours are unequal, otherwise false.
     */
    bool operator!=(const Colour &other) const
    {
        return !(other == *this);
    }

    /** Red. */
    float r;

    /** Green. */
    float g;

    /** Blue. */
    float b;

    /** Alpha. */
    float a;
};

/**
 * Write a Colour to a stream, useful for debugging.
 *
 * @param out
 *   Stream to write to.
 *
 * @param c
 *   Colour to write to stream.
 *
 * @return
 *   Reference to input stream.
 */
inline std::ostream &operator<<(std::ostream &out, const Colour &c)
{
    out << "r: " << c.r << " "
        << "g: " << c.g << " "
        << "b: " << c.b << " "
        << "a: " << c.a;

    return out;
}

}
