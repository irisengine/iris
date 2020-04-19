#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of Buffer types.
 */
enum class BufferType : std::uint32_t
{
    // special case for when a graphics API does not differenciate between
    // Buffer types
    DONT_CARE,

    VERTEX_ATTRIBUTES,
    VERTEX_INDICES
};

}

