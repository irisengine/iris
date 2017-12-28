#pragma once

#include <cstdint>

namespace eng
{

/** Encapsulated whether a key was a down or up press */
enum class key_state : std::uint32_t
{
    DOWN,
    UP
};

/** Encapsulates all key presses */
enum class key : std::uint32_t
{
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    EQUAL,
    MINUS,
    RIGHT_BRACKET,
    LEFT_BRACKET,
    QUOTE,
    SEMI_COLON,
    BACKSLASH,
    COMMA,
    SLASH,
    PERIOD,
    GRAVE,
    KEYPAD_DECIMAL,
    KEYPAD_MULTIPLY,
    KEYPAD_PLUS,
    KEYPAD_CLEAR,
    KEYPAD_DIVIDE,
    KEYPAD_ENTER,
    KEYPAD_MINUS,
    KEYPAD_EQUALS,
    KEYPAD_0,
    KEYPAD_1,
    KEYPAD_2,
    KEYPAD_3,
    KEYPAD_4,
    KEYPAD_5,
    KEYPAD_6,
    KEYPAD_7,
    KEYPAD_8,
    KEYPAD_9,
    RETURN,
    TAB,
    SPACE,
    DELETE,
    ESCAPE,
    COMMAND,
    SHIFT,
    CAPS_LOCK,
    OPTION,
    CONTROL,
    RIGHT_SHIFT,
    RIGHT_OPTION,
    RIGHT_CONTROL,
    FUNCTION,
    VOLUME_UP,
    VOLUME_DOWN,
    MUTE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    HELP,
    HOME,
    FORWARD_DELETE,
    END,
    PAGE_UP,
    PAGE_DOWN,
    LEFT_ARROW,
    RIGHT_ARROW,
    DOWN_ARROW,
    UP_ARROW,
};

/**
 * Encapsulated a keyboard event. Stores the key that was pressed
 * as well as if the key was a down or up press
 */
struct keyboard_event
{
    /**
     * Constructor
     *
     * @param k
     *   The key that was pressed
     *
     *  @param type
     *    Down or up press
     */
    keyboard_event(const key k, const key_state state) noexcept
        : key(k),
          state(state)
    { };

    /** Key that was pressed */
    const key key;

    /** Down or up press */
    const key_state state;
};

}

