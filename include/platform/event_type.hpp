#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of possible event types.
 */
enum class event_type : std::uint32_t
{
    KEYBOARD,
    MOUSE
};

}

