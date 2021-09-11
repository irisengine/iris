#include "graphics/macos/macos_window.h"

#include <memory>

#import <Appkit/Appkit.h>
#import <Foundation/Foundation.h>

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#include "core/exception.h"
#include "core/root.h"
#include "events/keyboard_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "graphics/anti_aliasing_level.h"
#include "graphics/macos/metal_app_delegate.h"
#include "graphics/macos/opengl_app_delegate.h"
#include "graphics/metal/metal_renderer.h"
#include "graphics/opengl/opengl_renderer.h"
#include "graphics/render_target.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to convert an OS X Key code to an engine specific key.
 *
 * @param key_code
 *   OS X specific Key code.
 *
 * @returns
 *   Emgine specific Key representation.
 */
iris::Key macos_key_to_engine_Key(const std::uint16_t key_code)
{
    iris::Key key;

    switch (key_code)
    {
        case 0x00: key = iris::Key::A; break;
        case 0x01: key = iris::Key::S; break;
        case 0x02: key = iris::Key::D; break;
        case 0x03: key = iris::Key::F; break;
        case 0x04: key = iris::Key::H; break;
        case 0x05: key = iris::Key::G; break;
        case 0x06: key = iris::Key::Z; break;
        case 0x07: key = iris::Key::X; break;
        case 0x08: key = iris::Key::C; break;
        case 0x09: key = iris::Key::V; break;
        case 0x0B: key = iris::Key::B; break;
        case 0x0C: key = iris::Key::Q; break;
        case 0x0D: key = iris::Key::W; break;
        case 0x0E: key = iris::Key::E; break;
        case 0x0F: key = iris::Key::R; break;
        case 0x10: key = iris::Key::Y; break;
        case 0x11: key = iris::Key::T; break;
        case 0x12: key = iris::Key::NUM_1; break;
        case 0x13: key = iris::Key::NUM_2; break;
        case 0x14: key = iris::Key::NUM_3; break;
        case 0x15: key = iris::Key::NUM_4; break;
        case 0x16: key = iris::Key::NUM_6; break;
        case 0x17: key = iris::Key::NUM_5; break;
        case 0x18: key = iris::Key::EQUAL; break;
        case 0x19: key = iris::Key::NUM_9; break;
        case 0x1A: key = iris::Key::NUM_7; break;
        case 0x1B: key = iris::Key::MINUS; break;
        case 0x1C: key = iris::Key::NUM_8; break;
        case 0x1D: key = iris::Key::NUM_0; break;
        case 0x1E: key = iris::Key::RIGHT_BRACKET; break;
        case 0x1F: key = iris::Key::O; break;
        case 0x20: key = iris::Key::U; break;
        case 0x21: key = iris::Key::LEFT_BRACKET; break;
        case 0x22: key = iris::Key::I; break;
        case 0x23: key = iris::Key::P; break;
        case 0x24: key = iris::Key::RETURN; break;
        case 0x25: key = iris::Key::L; break;
        case 0x26: key = iris::Key::J; break;
        case 0x27: key = iris::Key::QUOTE; break;
        case 0x28: key = iris::Key::K; break;
        case 0x29: key = iris::Key::SEMI_COLON; break;
        case 0x2A: key = iris::Key::BACKSLASH; break;
        case 0x2B: key = iris::Key::COMMA; break;
        case 0x2C: key = iris::Key::SLASH; break;
        case 0x2D: key = iris::Key::N; break;
        case 0x2E: key = iris::Key::M; break;
        case 0x2F: key = iris::Key::PERIOD; break;
        case 0x30: key = iris::Key::TAB; break;
        case 0x31: key = iris::Key::SPACE; break;
        case 0x32: key = iris::Key::GRAVE; break;
        case 0x33: key = iris::Key::FORWARD_DELETE; break;
        case 0x35: key = iris::Key::ESCAPE; break;
        case 0x37: key = iris::Key::COMMAND; break;
        case 0x38: key = iris::Key::SHIFT; break;
        case 0x39: key = iris::Key::CAPS_LOCK; break;
        case 0x3A: key = iris::Key::OPTION; break;
        case 0x3B: key = iris::Key::CONTROL; break;
        case 0x3C: key = iris::Key::RIGHT_SHIFT; break;
        case 0x3D: key = iris::Key::RIGHT_OPTION; break;
        case 0x3E: key = iris::Key::RIGHT_CONTROL; break;
        case 0x3F: key = iris::Key::FUNCTION; break;
        case 0x40: key = iris::Key::F17; break;
        case 0x41: key = iris::Key::KEYPAD_DECIMAL; break;
        case 0x43: key = iris::Key::KEYPAD_MULTIPLY; break;
        case 0x45: key = iris::Key::KEYPAD_PLUS; break;
        case 0x47: key = iris::Key::KEYPAD_CLEAR; break;
        case 0x48: key = iris::Key::VOLUME_UP; break;
        case 0x49: key = iris::Key::VOLUME_DOWN; break;
        case 0x4A: key = iris::Key::MUTE; break;
        case 0x4B: key = iris::Key::KEYPAD_DIVIDE; break;
        case 0x4C: key = iris::Key::KEYPAD_ENTER; break;
        case 0x4E: key = iris::Key::KEYPAD_MINUS; break;
        case 0x4F: key = iris::Key::F18; break;
        case 0x50: key = iris::Key::F19; break;
        case 0x51: key = iris::Key::KEYPAD_EQUALS; break;
        case 0x52: key = iris::Key::KEYPAD_0; break;
        case 0x53: key = iris::Key::KEYPAD_1; break;
        case 0x54: key = iris::Key::KEYPAD_2; break;
        case 0x55: key = iris::Key::KEYPAD_3; break;
        case 0x56: key = iris::Key::KEYPAD_4; break;
        case 0x57: key = iris::Key::KEYPAD_5; break;
        case 0x58: key = iris::Key::KEYPAD_6; break;
        case 0x59: key = iris::Key::KEYPAD_7; break;
        case 0x5A: key = iris::Key::F20; break;
        case 0x5B: key = iris::Key::KEYPAD_8; break;
        case 0x5C: key = iris::Key::KEYPAD_9; break;
        case 0x60: key = iris::Key::F5; break;
        case 0x61: key = iris::Key::F6; break;
        case 0x62: key = iris::Key::F7; break;
        case 0x63: key = iris::Key::F3; break;
        case 0x64: key = iris::Key::F8; break;
        case 0x65: key = iris::Key::F9; break;
        case 0x67: key = iris::Key::F11; break;
        case 0x69: key = iris::Key::F13; break;
        case 0x6A: key = iris::Key::F16; break;
        case 0x6B: key = iris::Key::F14; break;
        case 0x6D: key = iris::Key::F10; break;
        case 0x6F: key = iris::Key::F12; break;
        case 0x71: key = iris::Key::F15; break;
        case 0x72: key = iris::Key::HELP; break;
        case 0x73: key = iris::Key::HOME; break;
        case 0x74: key = iris::Key::PAGE_UP; break;
        case 0x75: key = iris::Key::FORWARD_DELETE; break;
        case 0x76: key = iris::Key::F4; break;
        case 0x77: key = iris::Key::END; break;
        case 0x78: key = iris::Key::F2; break;
        case 0x79: key = iris::Key::PAGE_DOWN; break;
        case 0x7A: key = iris::Key::F1; break;
        case 0x7B: key = iris::Key::LEFT_ARROW; break;
        case 0x7C: key = iris::Key::RIGHT_ARROW; break;
        case 0x7D: key = iris::Key::DOWN_ARROW; break;
        case 0x7E: key = iris::Key::UP_ARROW; break;
        default: key = iris::Key::UNKNOWN;
    }

    return key;
}

/**
 * Helper method to handle native keyboard events.
 *
 * @param event
 *   Native Event object.
 */
iris::KeyboardEvent handle_keyboard_event(NSEvent *event)
{
    // extract the Key code from the event
    const std::uint16_t key_code = [event keyCode];

    // convert the NSEventType to our Event state
    const auto type = ([event type] == NSEventTypeKeyDown)
                          ? iris::KeyState::DOWN
                          : iris::KeyState::UP;

    // convert Key code and dispatch
    const auto key = macos_key_to_engine_Key(key_code);

    return {key, type};
}

/**
 * Helper method to handle native mouse events.
 *
 * @param event
 *   Native Event object.
 */
iris::MouseEvent handle_mouse_event(NSEvent *event)
{
    // get mouse delta
    std::int32_t dx = 0;
    std::int32_t dy = 0;
    ::CGGetLastMouseDelta(&dx, &dy);

    // convert and dispatch
    return {static_cast<float>(dx), static_cast<float>(dy)};
}

}

namespace iris
{

MacosWindow::MacosWindow(
    std::uint32_t width,
    std::uint32_t height,
    AntiAliasingLevel anti_aliasing_level)
    : Window(width, height)
{
    // get and/or create the application singleton
    NSApplication *app = [NSApplication sharedApplication];

    // this is an ordinary app
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // make this app the active app
    [NSApp activateIgnoringOtherApps:YES];

    id<NSApplicationDelegate> app_delegate = nullptr;

    const auto samples = static_cast<int>(anti_aliasing_level);

    const auto api = Root::graphics_api();

    // create a graphics api specific Renderer and app delegate
    if (api == "metal")
    {
        app_delegate = [[MetalAppDelegate alloc]
            initWithRect:NSMakeRect(0.0f, 0.0f, width_, height_)];
        renderer_ = std::make_unique<MetalRenderer>(width_, height_);
    }
    else if (api == "opengl")
    {
        app_delegate = [[OpenGLAppDelegate alloc]
            initWithRect:NSMakeRect(0.0f, 0.0f, width_, height_)
             withSamples:samples];
        renderer_ = std::make_unique<OpenGLRenderer>(
            width_, height_, anti_aliasing_level);
    }
    else
    {
        throw Exception("unsupported graphics api");
    }

    // check that we created the delegate
    if (app_delegate == nil)
    {
        throw Exception("failed to create AppDelegate");
    }

    // set the delegate
    [app setDelegate:app_delegate];

    // activate the app
    [app finishLaunching];

    [NSCursor hide];

    // opengl window won't render till we pump events, so we do that here as it
    // doesn't matter if we are using opengl or metal
    pump_event();

    LOG_ENGINE_INFO("window", "macos window created {} {}", width_, height_);
}

std::uint32_t MacosWindow::screen_scale() const
{
    auto *window = [[NSApp windows] firstObject];
    return static_cast<std::uint32_t>([[window screen] backingScaleFactor]);
}

std::optional<Event> MacosWindow::pump_event()
{
    std::optional<Event> evt{};

    NSEvent *event = nil;

    // flush next event
    event = [NSApp nextEventMatchingMask:NSEventMaskAny
                               untilDate:[NSDate distantPast]
                                  inMode:NSDefaultRunLoopMode
                                 dequeue:YES];

    if (event != nil)
    {
        // handle native event
        switch ([event type])
        {
            case NSEventTypeKeyDown: [[fallthrough]];
            case NSEventTypeKeyUp: evt = handle_keyboard_event(event); break;
            case NSEventTypeMouseMoved: evt = handle_mouse_event(event); break;
            case NSEventTypeLeftMouseDown:
                evt =
                    MouseButtonEvent{MouseButton::LEFT, MouseButtonState::DOWN};
                break;
            case NSEventTypeLeftMouseUp:
                evt = MouseButtonEvent{MouseButton::LEFT, MouseButtonState::UP};
                break;
            case NSEventTypeRightMouseDown:
                evt = MouseButtonEvent{
                    MouseButton::RIGHT, MouseButtonState::DOWN};
                break;
            case NSEventTypeRightMouseUp:
                evt =
                    MouseButtonEvent{MouseButton::RIGHT, MouseButtonState::UP};
                break;
            default: break;
        }

        // dispatch the Event to other objects, this stops us swallowing
        // all events and preventing anything else from receiving them
        [NSApp sendEvent:event];
    }

    return evt;
}

}
