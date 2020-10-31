#pragma once

#include <cstdint>

namespace iris
{

/**
 * Enumeration of possible Event types.
 */
enum class EventType : std::uint32_t
{
    KEYBOARD,
    MOUSE,
    MOUSE_BUTTON,
    TOUCH
};

}
