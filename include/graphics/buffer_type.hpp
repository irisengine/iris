#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of buffer types.
 */
enum class buffer_type : std::uint32_t
{
    // special case for when a graphics API does not differenciate between
    // buffer types
    DONT_CARE,

    VERTEX_ATTRIBUTES,
    VERTEX_INDICES
};

}

