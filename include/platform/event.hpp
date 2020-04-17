#pragma once

#include <cstdint>
#include <variant>

#include "core/exception.hpp"
#include "platform/keyboard_event.hpp"
#include "platform/mouse_event.hpp"
#include "platform/event_type.hpp"

namespace eng
{

/**
 * Class encapsulating a user input event. This provides common access to
 * specific event types e.g. keyboard_event. 
 */
class event
{
    public:

        /**
         * Construct a new keyboard event.
         *
         * @param e
         *  Keyboard event.
         */
        event(const keyboard_event e);

        /**
         * Construct a new mouse event.
         *
         * @param e
         *  Mouse event.
         */
        event(const mouse_event e);

        /**
         * Get type of event.
         *
         * @returns
         *  Event type.
         */
        event_type type() const;

        /**
         * Check if event is a keyboard event.
         *
         * @returns
         *   True if event is a keyboard event, else false.
         */
        bool is_key() const;

        /**
         * Check if this event is a specific key event.
         *
         * @param k
         *   Key to check.
         *
         * @returns
         *   True if event is a keyboard event and matches supplied key.
         */
        bool is_key(key k) const;

        /**
         * Check if this event is a specific key event and state.
         *
         * @param k
         *   Key to check.
         *
         * @param state
         *   State to check.
         *
         * @returns
         *   True if event is a keyboard event and matches supplied key and
         *   state.
         */
        bool is_key(key k, key_state state) const;

        /**
         * Get keyboard_event, will throw if wrong type.
         *
         * @returns
         *   Keyboard event.
         */
        keyboard_event key() const;

        /**
         * Check if event is a mouse event.
         *
         * @returns
         *   True if event is a mouse event, else false.
         */
        bool is_mouse() const;

        /**
         * Get mouse_event, will throw if wrong type.
         *
         * @returns
         *   Mouse event.
         */
        mouse_event mouse() const;

    private:

        /** Type of event. */
        event_type type_;

        /** Variant of possible event types. */
        std::variant<keyboard_event, mouse_event> event_;
};

}
