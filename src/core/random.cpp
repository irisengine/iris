#include "core/random.h"

#include <random>

namespace
{

// ensure each thread gets its own random_device
thread_local std::random_device device;

}

namespace iris
{

std::uint32_t random_uint32(std::uint32_t min, std::uint32_t max)
{
    std::mt19937 engine(device());

    std::uniform_int_distribution<> dist(min, max);

    return dist(engine);
}

float random_float(float min, float max)
{
    std::mt19937 engine(device());

    std::uniform_real_distribution<> dist(min, max);

    return dist(engine);
}

}
