////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "events/event.h"

#include "core/error_handling.h"
#include "events/event_type.h"
#include "events/keyboard_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "events/quit_event.h"
#include "events/scroll_wheel_event.h"
#include "events/touch_event.h"

namespace iris
{

Event::Event(QuitEvent event)
    : type_(EventType::QUIT)
    , event_(event)
{
}

Event::Event(const KeyboardEvent event)
    : type_(EventType::KEYBOARD)
    , event_(event)
{
}

Event::Event(const MouseEvent event)
    : type_(EventType::MOUSE)
    , event_(event)
{
}

Event::Event(MouseButtonEvent event)
    : type_(EventType::MOUSE_BUTTON)
    , event_(event)
{
}

Event::Event(TouchEvent event)
    : type_(EventType::TOUCH)
    , event_(event)
{
}

Event::Event(ScrollWheelEvent event)
    : type_(EventType::SCROLL_WHEEL)
    , event_(event)
{
}

EventType Event::type() const
{
    return type_;
}

bool Event::is_quit() const
{
    return std::holds_alternative<QuitEvent>(event_);
}

bool Event::is_key() const
{
    return std::holds_alternative<KeyboardEvent>(event_);
}

bool Event::is_key(Key key) const
{
    auto match = false;

    if (auto val = std::get_if<KeyboardEvent>(&event_); val)
    {
        match = val->key == key;
    }

    return match;
}

bool Event::is_key(Key key, KeyState state) const
{
    auto match = false;

    if (auto val = std::get_if<KeyboardEvent>(&event_); val)
    {
        match = (val->key) == key && (val->state == state);
    }

    return match;
}

KeyboardEvent Event::key() const
{
    expect(is_key(), "not keyboard event");

    return std::get<KeyboardEvent>(event_);
}

bool Event::is_mouse() const
{
    return std::holds_alternative<MouseEvent>(event_);
}

MouseEvent Event::mouse() const
{
    expect(is_mouse(), "not mouse event");

    return std::get<MouseEvent>(event_);
}

bool Event::is_mouse_button() const
{
    return std::holds_alternative<MouseButtonEvent>(event_);
}

bool Event::is_mouse_button(MouseButton button) const
{
    auto match = false;

    if (auto val = std::get_if<MouseButtonEvent>(&event_); val)
    {
        match = val->button == button;
    }

    return match;
}

bool Event::is_mouse_button(MouseButton button, MouseButtonState state) const
{
    auto match = false;

    if (auto val = std::get_if<MouseButtonEvent>(&event_); val)
    {
        match = (val->button == button) && (val->state == state);
    }

    return match;
}

MouseButtonEvent Event::mouse_button() const
{
    expect(is_mouse_button(), "not mouse button event");

    return std::get<MouseButtonEvent>(event_);
}

bool Event::is_touch() const
{
    return std::holds_alternative<TouchEvent>(event_);
}

TouchEvent Event::touch() const
{
    expect(is_touch(), "not touch event");

    return std::get<TouchEvent>(event_);
}

bool Event::is_scroll_wheel() const
{
    return std::holds_alternative<ScrollWheelEvent>(event_);
}

ScrollWheelEvent Event::scroll_wheel() const
{
    return std::get<ScrollWheelEvent>(event_);
}

}
