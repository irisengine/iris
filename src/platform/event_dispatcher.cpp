#include "event_dispatcher.hpp"

#include "keyboard_event.hpp"
#include "log.hpp"
#include "mouse_event.hpp"

namespace eng
{

event_dispatcher::event_dispatcher(
    const keyboard_callback &keyboard_callback,
    const mouse_callback &mouse_callback) noexcept
    : keyboard_callback_(keyboard_callback),
      mouse_callback_(mouse_callback)
{
    LOG_INFO("event_dispatcher", "constructed");
}

void event_dispatcher::dispatch(const keyboard_event &event) const noexcept
{
    keyboard_callback_(event);
}

void event_dispatcher::dispatch(const mouse_event &event) const noexcept
{
    mouse_callback_(event);
}

}

