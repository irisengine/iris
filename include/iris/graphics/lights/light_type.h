#pragma once

#include <cstdint>

namespace iris
{

/**
 * Enumeration of possible light types.
 */
enum class LightType : std::uint8_t
{
    AMBIENT,
    DIRECTIONAL,
    POINT
};

}
