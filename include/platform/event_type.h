#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of possible Event types.
 */
enum class EventType : std::uint32_t
{
    KEYBOARD,
    MOUSE,
    TOUCH
};

}

