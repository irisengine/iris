#pragma once

#include <cstdint>

namespace iris
{
/** Encapsulates whether a mouse button we a down or up press. */
enum class MouseButtonState : std::uint32_t
{
    DOWN,
    UP
};

/** Encapsulates mouse buttons. */
enum class MouseButton : std::uint32_t
{
    LEFT,
    RIGHT
};

/**
 * Encapsulates a mouse button event.
 */
struct MouseButtonEvent
{
    /**
     * Construct a new MouseButtonEvent.
     *
     *
     * @param button
     *   Which mouse button was pressed.
     *
     * @param state
     *   Down or up press.
     */
    MouseButtonEvent(MouseButton button, MouseButtonState state)
        : button(button)
        , state(state)
    {
    }

    /** Mouse button pressed. */
    MouseButton button;

    /* Down or up press. */
    MouseButtonState state;
};
}
