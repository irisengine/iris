#include "platform/event.hpp"

#include "platform/event_type.hpp"
#include "platform/keyboard_event.hpp"
#include "platform/mouse_event.hpp"

namespace eng
{

event::event(const keyboard_event e)
    : type_(event_type::KEYBOARD),
      event_(e)
{ }

event::event(const mouse_event e)
    : type_(event_type::MOUSE),
      event_(e)
{ }

event_type event::type() const
{
    return type_;
}

bool event::is_key() const
{
    return std::holds_alternative<keyboard_event>(event_);
}

bool event::is_key(enum key k) const
{
    auto match = false;

    if(auto val = std::get_if<keyboard_event>(&event_) ; val)
    {
        match = val->key == k;
    }

    return match;
}

bool event::is_key(enum key k, key_state state) const
{
    auto match = false;

    if(auto val = std::get_if<keyboard_event>(&event_) ; val)
    {
        match = (val->key) == k && (val->state == state);
    }

    return match;
}

keyboard_event event::key() const
{
    if(!is_key())
    {
        throw exception("not keyboard event");
    }

    return std::get<keyboard_event>(event_);
}

bool event::is_mouse() const
{
    return std::holds_alternative<mouse_event>(event_);
}

mouse_event event::mouse() const
{
    if(!is_mouse())
    {
        throw exception("not mouse event");
    }

    return std::get<mouse_event>(event_);
}

}

