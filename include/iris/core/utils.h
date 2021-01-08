#pragma once

#include <cstdint>

namespace iris
{

/**
 * Compare two floating point numbers using a scaling epsilon.
 *
 * @param a
 *   First float.
 *
 * @param b
 *   Second float.
 *
 * @returns
 *   True if both floats are equal (within an epsilon), false otherwise.
 */
bool compare(float a, float b);

/**
 * Get the natural scale for the screen. This value reflects the scale factor
 * needed to convert from the default logical coordinate space into the device
 * coordinate space of this screen.
 *
 * @returns
 *   Screen scale factor.
 */
std::uint32_t screen_scale();

}
