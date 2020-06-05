#include "platform/window.h"

#include <memory>

#import <Appkit/Appkit.h>
#import <Foundation/Foundation.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#include "core/exception.h"
#include "core/real.h"
#include "log/log.h"
#include "platform/keyboard_event.h"
#include "platform/macos/AppDelegate.h"

namespace
{

/**
* Helper function to convert an OS X Key code to an engine specific key.
*
* @param key_code
*   OS X specific Key code.
*
* @returns
*   Enging specific Key representation.
*/
eng::Key macos_key_to_engine_Key(const std::uint16_t key_code)
{
    eng::Key key;

    switch(key_code)
    {
        case 0x00: key = eng::Key::A;                break;
        case 0x01: key = eng::Key::S;                break;
        case 0x02: key = eng::Key::D;                break;
        case 0x03: key = eng::Key::F;                break;
        case 0x04: key = eng::Key::H;                break;
        case 0x05: key = eng::Key::G;                break;
        case 0x06: key = eng::Key::Z;                break;
        case 0x07: key = eng::Key::X;                break;
        case 0x08: key = eng::Key::C;                break;
        case 0x09: key = eng::Key::V;                break;
        case 0x0B: key = eng::Key::B;                break;
        case 0x0C: key = eng::Key::Q;                break;
        case 0x0D: key = eng::Key::W;                break;
        case 0x0E: key = eng::Key::E;                break;
        case 0x0F: key = eng::Key::R;                break;
        case 0x10: key = eng::Key::Y;                break;
        case 0x11: key = eng::Key::T;                break;
        case 0x12: key = eng::Key::NUM_1;            break;
        case 0x13: key = eng::Key::NUM_2;            break;
        case 0x14: key = eng::Key::NUM_3;            break;
        case 0x15: key = eng::Key::NUM_4;            break;
        case 0x16: key = eng::Key::NUM_6;            break;
        case 0x17: key = eng::Key::NUM_5;            break;
        case 0x18: key = eng::Key::EQUAL;            break;
        case 0x19: key = eng::Key::NUM_9;            break;
        case 0x1A: key = eng::Key::NUM_7;            break;
        case 0x1B: key = eng::Key::MINUS;            break;
        case 0x1C: key = eng::Key::NUM_8;            break;
        case 0x1D: key = eng::Key::NUM_0;            break;
        case 0x1E: key = eng::Key::RIGHT_BRACKET;    break;
        case 0x1F: key = eng::Key::O;                break;
        case 0x20: key = eng::Key::U;                break;
        case 0x21: key = eng::Key::LEFT_BRACKET;     break;
        case 0x22: key = eng::Key::I;                break;
        case 0x23: key = eng::Key::P;                break;
        case 0x24: key = eng::Key::RETURN;           break;
        case 0x25: key = eng::Key::L;                break;
        case 0x26: key = eng::Key::J;                break;
        case 0x27: key = eng::Key::QUOTE;            break;
        case 0x28: key = eng::Key::K;                break;
        case 0x29: key = eng::Key::SEMI_COLON;       break;
        case 0x2A: key = eng::Key::BACKSLASH;        break;
        case 0x2B: key = eng::Key::COMMA;            break;
        case 0x2C: key = eng::Key::SLASH;            break;
        case 0x2D: key = eng::Key::N;                break;
        case 0x2E: key = eng::Key::M;                break;
        case 0x2F: key = eng::Key::PERIOD;           break;
        case 0x30: key = eng::Key::TAB;              break;
        case 0x31: key = eng::Key::SPACE;            break;
        case 0x32: key = eng::Key::GRAVE;            break;
        case 0x33: key = eng::Key::DELETE;           break;
        case 0x35: key = eng::Key::ESCAPE;           break;
        case 0x37: key = eng::Key::COMMAND;          break;
        case 0x38: key = eng::Key::SHIFT;            break;
        case 0x39: key = eng::Key::CAPS_LOCK;        break;
        case 0x3A: key = eng::Key::OPTION;           break;
        case 0x3B: key = eng::Key::CONTROL;          break;
        case 0x3C: key = eng::Key::RIGHT_SHIFT;      break;
        case 0x3D: key = eng::Key::RIGHT_OPTION;     break;
        case 0x3E: key = eng::Key::RIGHT_CONTROL;    break;
        case 0x3F: key = eng::Key::FUNCTION;         break;
        case 0x40: key = eng::Key::F17;              break;
        case 0x41: key = eng::Key::KEYPAD_DECIMAL;   break;
        case 0x43: key = eng::Key::KEYPAD_MULTIPLY;  break;
        case 0x45: key = eng::Key::KEYPAD_PLUS;      break;
        case 0x47: key = eng::Key::KEYPAD_CLEAR;     break;
        case 0x48: key = eng::Key::VOLUME_UP;        break;
        case 0x49: key = eng::Key::VOLUME_DOWN;      break;
        case 0x4A: key = eng::Key::MUTE;             break;
        case 0x4B: key = eng::Key::KEYPAD_DIVIDE;    break;
        case 0x4C: key = eng::Key::KEYPAD_ENTER;     break;
        case 0x4E: key = eng::Key::KEYPAD_MINUS;     break;
        case 0x4F: key = eng::Key::F18;              break;
        case 0x50: key = eng::Key::F19;              break;
        case 0x51: key = eng::Key::KEYPAD_EQUALS;    break;
        case 0x52: key = eng::Key::KEYPAD_0;         break;
        case 0x53: key = eng::Key::KEYPAD_1;         break;
        case 0x54: key = eng::Key::KEYPAD_2;         break;
        case 0x55: key = eng::Key::KEYPAD_3;         break;
        case 0x56: key = eng::Key::KEYPAD_4;         break;
        case 0x57: key = eng::Key::KEYPAD_5;         break;
        case 0x58: key = eng::Key::KEYPAD_6;         break;
        case 0x59: key = eng::Key::KEYPAD_7;         break;
        case 0x5A: key = eng::Key::F20;              break;
        case 0x5B: key = eng::Key::KEYPAD_8;         break;
        case 0x5C: key = eng::Key::KEYPAD_9;         break;
        case 0x60: key = eng::Key::F5;               break;
        case 0x61: key = eng::Key::F6;               break;
        case 0x62: key = eng::Key::F7;               break;
        case 0x63: key = eng::Key::F3;               break;
        case 0x64: key = eng::Key::F8;               break;
        case 0x65: key = eng::Key::F9;               break;
        case 0x67: key = eng::Key::F11;              break;
        case 0x69: key = eng::Key::F13;              break;
        case 0x6A: key = eng::Key::F16;              break;
        case 0x6B: key = eng::Key::F14;              break;
        case 0x6D: key = eng::Key::F10;              break;
        case 0x6F: key = eng::Key::F12;              break;
        case 0x71: key = eng::Key::F15;              break;
        case 0x72: key = eng::Key::HELP;             break;
        case 0x73: key = eng::Key::HOME;             break;
        case 0x74: key = eng::Key::PAGE_UP;          break;
        case 0x75: key = eng::Key::FORWARD_DELETE;   break;
        case 0x76: key = eng::Key::F4;               break;
        case 0x77: key = eng::Key::END;              break;
        case 0x78: key = eng::Key::F2;               break;
        case 0x79: key = eng::Key::PAGE_DOWN;        break;
        case 0x7A: key = eng::Key::F1;               break;
        case 0x7B: key = eng::Key::LEFT_ARROW;       break;
        case 0x7C: key = eng::Key::RIGHT_ARROW;      break;
        case 0x7D: key = eng::Key::DOWN_ARROW;       break;
        case 0x7E: key = eng::Key::UP_ARROW;         break;
        default : throw eng::Exception("unknown Key type");
    }

    return key;
}

/**
 * Helper method to handle native keyboard events.
 *
 * @param event
 *   Native Event object.
 */
eng::KeyboardEvent handle_keyboard_event(NSEvent *event)
{
    // extract the Key code from the event
    const std::uint16_t key_code = [event keyCode];

    // convert the NSEventType to our Event state
    const auto type = ([event type] == NSEventTypeKeyDown)
        ? eng::KeyState::DOWN
        : eng::KeyState::UP;

    // convert Key code and dispatch
    const auto key = macos_key_to_engine_Key(key_code);

    return { key, type };
}

/**
 * Helper method to handle native mouse events.
 *
 * @param event
 *   Native Event object.
 */
eng::MouseEvent handle_mouse_event(NSEvent *event)
{
    // get mouse delta
    std::int32_t dx = 0;
    std::int32_t dy = 0;
    ::CGGetLastMouseDelta(&dx, &dy);

    // convert and dispatch
    return { static_cast<eng::real>(dx), static_cast<eng::real>(dy) };
}

}

namespace eng
{

Window::Window(real width, real height)
    : width_(width),
      height_(height)
{
    // get and/or create the application singleton
    NSApplication *app = [NSApplication sharedApplication];

    // this is an ordinary app
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // make this app the active app
    [NSApp activateIgnoringOtherApps:YES];

    // create our app delegate, this will handle the Window creation
    AppDelegate *appDelegate = [[AppDelegate alloc] initWithRect:NSMakeRect(0.0f, 0.0f, width_, height_)];

    // check that we created the delegate
    if(appDelegate == nil)
    {
        throw Exception("failed to create AppDelegate");
    }

    // set the delegate
    [app setDelegate:appDelegate];

    // activate the app
    [app finishLaunching];

    [NSCursor hide];

    LOG_ENGINE_INFO("window", "macos window created {} {}", width_, height_);
}

std::optional<Event> Window::pump_event()
{
    std::optional<Event> evt{ };

    NSEvent *event = nil;

    // flush next event
    event = [NSApp
        nextEventMatchingMask:NSEventMaskAny
        untilDate:[NSDate distantPast]
        inMode:NSDefaultRunLoopMode
        dequeue:YES];

    if(event != nil)
    {
        // handle native event
        switch([event type])
        {
            case NSEventTypeKeyDown: [[fallthrough]];
            case NSEventTypeKeyUp:
                evt = handle_keyboard_event(event);
                break;
            case NSEventTypeMouseMoved:
                evt = handle_mouse_event(event);
                break;
            default:
                break;
        }

        // dispatch the Event to other objects, this stops us swallowing
        // all events and preventing anything else from receiving them
        [NSApp sendEvent:event];
    }

    return evt;
}

real Window::width() const
{
    return width_;
}

real Window::height() const
{
    return height_;
}

}

