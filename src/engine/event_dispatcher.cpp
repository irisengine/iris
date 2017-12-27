#include "event_dispatcher.hpp"

#include "keyboard_event.hpp"

namespace eng
{

event_dispatcher::event_dispatcher(const keyboard_callback &keyboard_callback) noexcept
    : keyboard_callback_(keyboard_callback)
{ }

void event_dispatcher::dispatch(const keyboard_event &event) const noexcept
{
    keyboard_callback_(event);
}

}

