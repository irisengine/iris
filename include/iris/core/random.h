#pragma once

#include <cstdint>

namespace iris
{

/**
 * Generate a uniform random integer in the range [min, max].
 *
 * @param min
 *   Minimum value.
 *
 * @param max
 *   Maximum value.
 *
 * @returns
 *   Random integer.
 */
std::uint32_t random_uint32(std::uint32_t min, std::uint32_t max);

/**
 * Generate a uniform random float in the range [min, max).
 *
 * @param min
 *   Minimum value.
 *
 * @param max
 *   Maximum value.
 *
 * @returns
 *   Random integer.
 */
float random_float(float min, float max);

}
