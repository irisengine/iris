////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <variant>

#include "core/exception.h"
#include "events/event_type.h"
#include "events/keyboard_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "events/quit_event.h"
#include "events/touch_event.h"

namespace iris
{

/**
 * Class encapsulating a user input event. This provides common access to
 * specific Event types e.g. keyboard_event.
 */
class Event
{
  public:
    /**
     * Construct a new Quit event.
     *
     * @param event
     *   Quit event.
     */
    Event(QuitEvent event);
    /**
     * Construct a new keyboard event.
     *
     * @param event
     *   Keyboard event.
     */
    Event(KeyboardEvent event);

    /**
     * Construct a new mouse event.
     *
     * @param event
     *   Mouse event.
     */
    Event(MouseEvent event);

    /**
     * Construct a new mouse button event.
     *
     * @param event
     *   Mouse button event.
     */
    Event(MouseButtonEvent event);

    /**
     * Construct a new touch event.
     *
     * @param event
     *   Touch event.
     */
    Event(TouchEvent event);

    /**
     * Get type of event.
     *
     * @returns
     *   Event type.
     */
    EventType type() const;

    /**
     * Check if Event is a quit event.
     *
     * @returns
     *   True if this Event is a quit event, else false.
     */
    bool is_quit() const;

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
     * Get keyboard event, will throw if wrong type.
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
     * Get mouse event, will throw if wrong type.
     *
     * @returns
     *   Mouse event.
     */
    MouseEvent mouse() const;

    /**
     * Check if event is a mouse button event.
     *
     * @returns
     *   True if Event is a mouse button event, else false.
     */
    bool is_mouse_button() const;

    /**
     * Check if this event is a specific mouse button event
     *
     * @param button
     *   Mouse button to check.
     *
     * @returns
     *   True if Event is a mouse button Event and matches supplied button.
     */
    bool is_mouse_button(MouseButton button) const;

    /**
     * Check if this event is a specific mouse button event and state
     *
     * @param button
     *   Mouse button to check.
     *
     * @param state
     *   State to check.
     *
     * @returns
     *   True if Event is a mouse button Event and matches supplied button and
     *   state.
     */
    bool is_mouse_button(MouseButton button, MouseButtonState state) const;

    /**
     * Get mouse button event, will throw if wrong type.
     *
     * @returns
     *   Mouse button event.
     */
    MouseButtonEvent mouse_button() const;

    /**
     * Check if Event is a touch event.
     *
     * @returns
     *   True if Event is a touch event, else false.
     */
    bool is_touch() const;

    /**
     * Get touch event will throw if wrong type.
     *
     * @returns
     *   Touch event.
     */
    TouchEvent touch() const;

  private:
    /** Type of event. */
    EventType type_;

    /** Variant of possible Event types. */
    std::variant<
        QuitEvent,
        KeyboardEvent,
        MouseEvent,
        MouseButtonEvent,
        TouchEvent>
        event_;
};

}
