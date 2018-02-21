#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of supported rigid body shapes.
 */
enum class rigid_body_shape : std::uint64_t
{
    PLANE,
    BOX
};

}

