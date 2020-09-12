#pragma once

#include <cstdint>

namespace iris
{

/**
 * Enumeration of camera types.
 */
enum class CameraType : std::uint8_t
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

}
