#include "platform/event_dispatcher.hpp"

#include "log/log.hpp"
#include "platform/keyboard_event.hpp"
#include "platform/mouse_event.hpp"

namespace eng
{

event_dispatcher::event_dispatcher(
    const keyboard_callback &keyboard_callback,
    const mouse_callback &mouse_callback)
    : keyboard_callback_(keyboard_callback),
      mouse_callback_(mouse_callback)
{
    LOG_INFO("event_dispatcher", "constructed");
}

void event_dispatcher::dispatch(const keyboard_event &event) const
{
    keyboard_callback_(event);
}

void event_dispatcher::dispatch(const mouse_event &event) const
{
    mouse_callback_(event);
}

}

