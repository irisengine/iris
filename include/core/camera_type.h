#pragma once

#include <cstdint>

namespace eng
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

