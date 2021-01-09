#include "core/random.h"

#include <random>

namespace
{

// ensure each thread gets its own random_device
thread_local std::random_device device;

/**
 * Helper function to generate a random value with a given distribution.
 * 
 * @param distribution
 *   Distribution of random value.
 * 
 * @returns
 *   Random value with given distribution.
 */
template<class T>
typename T::result_type generate(T distribution)
{
    std::mt19937 engine(device());
    return distribution(engine);
}

}

namespace iris
{

std::uint32_t random_uint32(std::uint32_t min, std::uint32_t max)
{
    return generate(std::uniform_int_distribution<>(min, max));
}

std::int32_t random_int32(std::int32_t min, std::int32_t max)
{
    return generate(std::uniform_int_distribution<>(min, max));
}

float random_float(float min, float max)
{
    return generate(std::uniform_real_distribution<float>(min, max));
}

bool flip_coin(float bias)
{
    return generate(std::bernoulli_distribution(bias));
}

}
