#pragma once

#include <cstdint>
#include <ostream>

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
    Colour();

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
    Colour(float r, float g, float b, float a = 1.0f);

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
    Colour(
        std::int32_t r,
        std::int32_t g,
        std::int32_t b,
        std::int32_t a = 255);

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
    friend std::ostream &operator<<(std::ostream &out, const Colour &c);

    /**
     * Multiply each component by a scalar value.
     *
     * @param scale
     *   Scalar value.
     *
     * @return
     *   Reference to this Colour.
     */
    Colour &operator*=(float scale);

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
    Colour operator*(float scale) const;

    /**
     * Component wise add a Colour to this Colour.
     *
     * @param colour
     *   The Colour to add to this.
     *
     * @return
     *   Reference to this colour.
     */
    Colour &operator+=(const Colour &colour);

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
    Colour operator+(const Colour &colour) const;

    /**
     * Component wise subtract a Colour to this Colour.
     *
     * @param v
     *   The Colour to subtract from this.
     *
     * @return
     *   Reference to this Colour.
     */
    Colour &operator-=(const Colour &colour);

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
    Colour operator-(const Colour &colour) const;

    /**
     * Component wise multiple a Colour to this Colour.
     *
     * @param colour
     *   The Colour to multiply.
     *
     * @returns
     *   Reference to this Colour.
     */
    Colour &operator*=(const Colour &colour);

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
    Colour operator*(const Colour &colour) const;

    /**
     * Check if this colour is equal to another.
     *
     * @param other
     *   Colour to compare with.
     *
     * @returns
     *   True if both Colours are equal, otherwise false.
     */
    bool operator==(const Colour &other) const;

    /**
     * Check if this colour is not equal to another.
     *
     * @param other
     *   Colour to compare with.
     *
     * @returns
     *   True if both Colours are unequal, otherwise false.
     */
    bool operator!=(const Colour &other) const;

    /** Red. */
    float r;

    /** Green. */
    float g;

    /** Blue. */
    float b;

    /** Alpha. */
    float a;
};
}
