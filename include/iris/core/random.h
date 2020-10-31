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

/**
 * Get a random element from a collection.
 *
 * @param collection
 *   Collection to get element from.
 *
 * @returns
 *   Random element.
 */
template <class T>
typename T::reference random_element(T &container)
{
    const auto index = random_uint32(0u, container.size() - 1u);
    return container[index];
}

/**
 * Get a random element from a collection.
 *
 * @param collection
 *   Collection to get element from.
 *
 * @returns
 *   Random element.
 */
template <class T>
typename T::const_reference random_element(const T &container)
{
    const auto index = random_uint32(0u, container.size() - 1u);
    return container[index];
}

}
