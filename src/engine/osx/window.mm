#include "window.hpp"

#include <stdexcept>

#import <Appkit/Appkit.h>
#import <Foundation/Foundation.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#include "event_dispatcher.hpp"
#include "keyboard_event.hpp"
#include "osx/AppDelegate.h"

namespace
{

/**
* Helper function to convert an OS X key code to an engine specific key.
*
* @param key_code
*   OS X specific key code.
*
* @returns
*   Enging specific key representation.
*/
eng::key osx_key_to_engine_key(const std::uint16_t key_code)
{
    eng::key key;

    switch(key_code)
    {
        case 0x00: key = eng::key::A;                break;
        case 0x01: key = eng::key::S;                break;
        case 0x02: key = eng::key::D;                break;
        case 0x03: key = eng::key::F;                break;
        case 0x04: key = eng::key::H;                break;
        case 0x05: key = eng::key::G;                break;
        case 0x06: key = eng::key::Z;                break;
        case 0x07: key = eng::key::X;                break;
        case 0x08: key = eng::key::C;                break;
        case 0x09: key = eng::key::V;                break;
        case 0x0B: key = eng::key::B;                break;
        case 0x0C: key = eng::key::Q;                break;
        case 0x0D: key = eng::key::W;                break;
        case 0x0E: key = eng::key::E;                break;
        case 0x0F: key = eng::key::R;                break;
        case 0x10: key = eng::key::Y;                break;
        case 0x11: key = eng::key::T;                break;
        case 0x12: key = eng::key::NUM_1;            break;
        case 0x13: key = eng::key::NUM_2;            break;
        case 0x14: key = eng::key::NUM_3;            break;
        case 0x15: key = eng::key::NUM_4;            break;
        case 0x16: key = eng::key::NUM_6;            break;
        case 0x17: key = eng::key::NUM_5;            break;
        case 0x18: key = eng::key::EQUAL;            break;
        case 0x19: key = eng::key::NUM_9;            break;
        case 0x1A: key = eng::key::NUM_7;            break;
        case 0x1B: key = eng::key::MINUS;            break;
        case 0x1C: key = eng::key::NUM_8;            break;
        case 0x1D: key = eng::key::NUM_0;            break;
        case 0x1E: key = eng::key::RIGHT_BRACKET;    break;
        case 0x1F: key = eng::key::O;                break;
        case 0x20: key = eng::key::U;                break;
        case 0x21: key = eng::key::LEFT_BRACKET;     break;
        case 0x22: key = eng::key::I;                break;
        case 0x23: key = eng::key::P;                break;
        case 0x24: key = eng::key::RETURN;           break;
        case 0x25: key = eng::key::L;                break;
        case 0x26: key = eng::key::J;                break;
        case 0x27: key = eng::key::QUOTE;            break;
        case 0x28: key = eng::key::K;                break;
        case 0x29: key = eng::key::SEMI_COLON;       break;
        case 0x2A: key = eng::key::BACKSLASH;        break;
        case 0x2B: key = eng::key::COMMA;            break;
        case 0x2C: key = eng::key::SLASH;            break;
        case 0x2D: key = eng::key::N;                break;
        case 0x2E: key = eng::key::M;                break;
        case 0x2F: key = eng::key::PERIOD;           break;
        case 0x30: key = eng::key::TAB;              break;
        case 0x31: key = eng::key::SPACE;            break;
        case 0x32: key = eng::key::GRAVE;            break;
        case 0x33: key = eng::key::DELETE;           break;
        case 0x35: key = eng::key::ESCAPE;           break;
        case 0x37: key = eng::key::COMMAND;          break;
        case 0x38: key = eng::key::SHIFT;            break;
        case 0x39: key = eng::key::CAPS_LOCK;        break;
        case 0x3A: key = eng::key::OPTION;           break;
        case 0x3B: key = eng::key::CONTROL;          break;
        case 0x3C: key = eng::key::RIGHT_SHIFT;      break;
        case 0x3D: key = eng::key::RIGHT_OPTION;     break;
        case 0x3E: key = eng::key::RIGHT_CONTROL;    break;
        case 0x3F: key = eng::key::FUNCTION;         break;
        case 0x40: key = eng::key::F17;              break;
        case 0x41: key = eng::key::KEYPAD_DECIMAL;   break;
        case 0x43: key = eng::key::KEYPAD_MULTIPLY;  break;
        case 0x45: key = eng::key::KEYPAD_PLUS;      break;
        case 0x47: key = eng::key::KEYPAD_CLEAR;     break;
        case 0x48: key = eng::key::VOLUME_UP;        break;
        case 0x49: key = eng::key::VOLUME_DOWN;      break;
        case 0x4A: key = eng::key::MUTE;             break;
        case 0x4B: key = eng::key::KEYPAD_DIVIDE;    break;
        case 0x4C: key = eng::key::KEYPAD_ENTER;     break;
        case 0x4E: key = eng::key::KEYPAD_MINUS;     break;
        case 0x4F: key = eng::key::F18;              break;
        case 0x50: key = eng::key::F19;              break;
        case 0x51: key = eng::key::KEYPAD_EQUALS;    break;
        case 0x52: key = eng::key::KEYPAD_0;         break;
        case 0x53: key = eng::key::KEYPAD_1;         break;
        case 0x54: key = eng::key::KEYPAD_2;         break;
        case 0x55: key = eng::key::KEYPAD_3;         break;
        case 0x56: key = eng::key::KEYPAD_4;         break;
        case 0x57: key = eng::key::KEYPAD_5;         break;
        case 0x58: key = eng::key::KEYPAD_6;         break;
        case 0x59: key = eng::key::KEYPAD_7;         break;
        case 0x5A: key = eng::key::F20;              break;
        case 0x5B: key = eng::key::KEYPAD_8;         break;
        case 0x5C: key = eng::key::KEYPAD_9;         break;
        case 0x60: key = eng::key::F5;               break;
        case 0x61: key = eng::key::F6;               break;
        case 0x62: key = eng::key::F7;               break;
        case 0x63: key = eng::key::F3;               break;
        case 0x64: key = eng::key::F8;               break;
        case 0x65: key = eng::key::F9;               break;
        case 0x67: key = eng::key::F11;              break;
        case 0x69: key = eng::key::F13;              break;
        case 0x6A: key = eng::key::F16;              break;
        case 0x6B: key = eng::key::F14;              break;
        case 0x6D: key = eng::key::F10;              break;
        case 0x6F: key = eng::key::F12;              break;
        case 0x71: key = eng::key::F15;              break;
        case 0x72: key = eng::key::HELP;             break;
        case 0x73: key = eng::key::HOME;             break;
        case 0x74: key = eng::key::PAGE_UP;          break;
        case 0x75: key = eng::key::FORWARD_DELETE;   break;
        case 0x76: key = eng::key::F4;               break;
        case 0x77: key = eng::key::END;              break;
        case 0x78: key = eng::key::F2;               break;
        case 0x79: key = eng::key::PAGE_DOWN;        break;
        case 0x7A: key = eng::key::F1;               break;
        case 0x7B: key = eng::key::LEFT_ARROW;       break;
        case 0x7C: key = eng::key::RIGHT_ARROW;      break;
        case 0x7D: key = eng::key::DOWN_ARROW;       break;
        case 0x7E: key = eng::key::UP_ARROW;         break;
        default : throw std::runtime_error("unknown key type");
    }

    return key;
}

}

namespace eng
{

window::window(
    event_dispatcher &dispatcher,
    const float width,
    const float height)
    : dispatcher_(dispatcher),
      width_(width),
      height_(height)
{
    // get and/or create the application singleton
    NSApplication *app = [NSApplication sharedApplication];

    // this is an ordinary app
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // make this app the active app
    [NSApp activateIgnoringOtherApps:YES];

    // create our app delegate, this will handle the window creation
    AppDelegate *appDelegate = [[AppDelegate alloc] initWithRect:NSMakeRect(0.0f, 0.0f, width_, height_)];

    // check that we created the delegate
    if(appDelegate == nil)
    {
        throw std::runtime_error("failed to create AppDelegate");
    }

    // set the delegate
    [app setDelegate:appDelegate];

    // activate the app
    [app finishLaunching];

    // create an auto release pool for this thread, according to the Apple
    // docs this will get automatically drained when the main thread
    // terminates
    // http://tinyurl.com/nmdem83
    [[NSAutoreleasePool alloc] init];

    [NSCursor hide];

    // perform OpenGl setup
    ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void window::pre_render() const noexcept
{
    NSEvent *event = nil;

    do
    {
        // flush next event
        event = [NSApp
            nextEventMatchingMask:NSEventMaskAny
            untilDate:[NSDate distantPast]
            inMode:NSDefaultRunLoopMode
            dequeue:YES];

        // check if event is a keypress
        if(([event type] == NSEventTypeKeyDown) || ([event type] == NSEventTypeKeyUp))
        {
            // extract the key code from the event
            const std::uint16_t key_code = [event keyCode];

            // convert the NSEventType to our event state
            const auto type = ([event type] == NSEventTypeKeyDown)
                ? key_state::DOWN
                : key_state::UP;

            // convert key code and dispatch
            const auto key = osx_key_to_engine_key(key_code);
            dispatcher_.dispatch(keyboard_event{ key, type });
        }

        // dispatch the event to other objects, this stops us swallowing
        // all events and preventing anything else from receiving them
        [NSApp sendEvent:event];

    } while(event != nil);

    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window::post_render() const noexcept
{
    ::glSwapAPPLE();
}

float window::width() const noexcept
{
    return width_;
}

float window::height() const noexcept
{
    return height_;
}

}

