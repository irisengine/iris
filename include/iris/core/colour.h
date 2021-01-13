#pragma once

#include <cstdint>

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
