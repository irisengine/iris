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
 * specific Event types e.g. keyboard_event. 
 */
class Event
{
    public:

        /**
         * Construct a new keyboard event.
         *
         * @param event
         *  Keyboard event.
         */
        Event(const KeyboardEvent event);

        /**
         * Construct a new mouse event.
         *
         * @param event
         *  Mouse event.
         */
        Event(const MouseEvent event);

        /**
         * Get type of event.
         *
         * @returns
         *  Event type.
         */
        EventType type() const;

        /**
         * Check if Event is a keyboard event.
         *
         * @returns
         *   True if Event is a keyboard event, else false.
         */
        bool is_key() const;

        /**
         * Check if this Event is a specific Key event.
         *
         * @param key
         *   Key to check.
         *
         * @returns
         *   True if Event is a keyboard Event and matches supplied key.
         */
        bool is_key(Key key) const;

        /**
         * Check if this Event is a specific Key Event and state.
         *
         * @param key
         *   Key to check.
         *
         * @param state
         *   State to check.
         *
         * @returns
         *   True if Event is a keyboard Event and matches supplied Key and
         *   state.
         */
        bool is_key(Key key, KeyState state) const;

        /**
         * Get keyboard_event, will throw if wrong type.
         *
         * @returns
         *   Keyboard event.
         */
        KeyboardEvent key() const;

        /**
         * Check if Event is a mouse event.
         *
         * @returns
         *   True if Event is a mouse event, else false.
         */
        bool is_mouse() const;

        /**
         * Get mouse_event, will throw if wrong type.
         *
         * @returns
         *   Mouse event.
         */
        MouseEvent mouse() const;

    private:

        /** Type of event. */
        EventType type_;

        /** Variant of possible Event types. */
        std::variant<KeyboardEvent, MouseEvent> event_;
};

}
