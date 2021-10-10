////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/win32/win32_window.h"

#include <cmath>
#include <optional>
#include <queue>

#define WIN32_LEAN_AND_MEAN
#include <ShellScalingApi.h>
#include <Windows.h>
#include <Windowsx.h>
#include <hidusage.h>

#include "core/auto_release.h"
#include "core/error_handling.h"
#include "events/event.h"
#include "events/quit_event.h"

#pragma comment(lib, "Shcore.lib")

namespace
{

// as we have to provide a callback to windows for event data and there is no
// way of passing in custom data we use a global queue to store events
std::queue<iris::Event> event_queue;

/**
 * Helper function to convert a windows key code to an engine key type.
 *
 * @param key_code
 *   Windows key code.
 *
 * @returns
 *   Engine Key.
 */
iris::Key windows_key_to_engine_Key(WPARAM key_code)
{
    iris::Key key;

    switch (key_code)
    {
        case 0x30: key = iris::Key::NUM_0; break;
        case 0x31: key = iris::Key::NUM_1; break;
        case 0x32: key = iris::Key::NUM_2; break;
        case 0x33: key = iris::Key::NUM_3; break;
        case 0x34: key = iris::Key::NUM_4; break;
        case 0x35: key = iris::Key::NUM_5; break;
        case 0x36: key = iris::Key::NUM_6; break;
        case 0x37: key = iris::Key::NUM_7; break;
        case 0x38: key = iris::Key::NUM_8; break;
        case 0x39: key = iris::Key::NUM_9; break;
        case 0x41: key = iris::Key::A; break;
        case 0x42: key = iris::Key::B; break;
        case 0x43: key = iris::Key::C; break;
        case 0x44: key = iris::Key::D; break;
        case 0x45: key = iris::Key::E; break;
        case 0x46: key = iris::Key::F; break;
        case 0x47: key = iris::Key::G; break;
        case 0x48: key = iris::Key::H; break;
        case 0x49: key = iris::Key::I; break;
        case 0x4a: key = iris::Key::J; break;
        case 0x4b: key = iris::Key::K; break;
        case 0x4c: key = iris::Key::L; break;
        case 0x4d: key = iris::Key::M; break;
        case 0x4e: key = iris::Key::N; break;
        case 0x4f: key = iris::Key::O; break;
        case 0x50: key = iris::Key::P; break;
        case 0x51: key = iris::Key::Q; break;
        case 0x52: key = iris::Key::R; break;
        case 0x53: key = iris::Key::S; break;
        case 0x54: key = iris::Key::T; break;
        case 0x55: key = iris::Key::U; break;
        case 0x56: key = iris::Key::V; break;
        case 0x57: key = iris::Key::W; break;
        case 0x58: key = iris::Key::X; break;
        case 0x59: key = iris::Key::Y; break;
        case 0x5a: key = iris::Key::Z; break;
        case VK_TAB: key = iris::Key::TAB; break;
        case VK_SPACE: key = iris::Key::SPACE; break;
        case VK_ESCAPE: key = iris::Key::ESCAPE; break;
        case VK_LSHIFT: key = iris::Key::SHIFT; break;
        case VK_RSHIFT: key = iris::Key::RIGHT_SHIFT; break;
        case VK_F17: key = iris::Key::F17; break;
        case VK_DECIMAL: key = iris::Key::KEYPAD_DECIMAL; break;
        case VK_MULTIPLY: key = iris::Key::KEYPAD_MULTIPLY; break;
        case VK_OEM_PLUS: key = iris::Key::KEYPAD_PLUS; break;
        case VK_VOLUME_UP: key = iris::Key::VOLUME_UP; break;
        case VK_VOLUME_DOWN: key = iris::Key::VOLUME_DOWN; break;
        case VK_VOLUME_MUTE: key = iris::Key::MUTE; break;
        case VK_DIVIDE: key = iris::Key::KEYPAD_DIVIDE; break;
        case VK_OEM_MINUS: key = iris::Key::KEYPAD_MINUS; break;
        case VK_F18: key = iris::Key::F18; break;
        case VK_F19: key = iris::Key::F19; break;
        case VK_NUMPAD0: key = iris::Key::KEYPAD_0; break;
        case VK_NUMPAD1: key = iris::Key::KEYPAD_1; break;
        case VK_NUMPAD2: key = iris::Key::KEYPAD_2; break;
        case VK_NUMPAD3: key = iris::Key::KEYPAD_3; break;
        case VK_NUMPAD4: key = iris::Key::KEYPAD_4; break;
        case VK_NUMPAD5: key = iris::Key::KEYPAD_5; break;
        case VK_NUMPAD6: key = iris::Key::KEYPAD_6; break;
        case VK_NUMPAD7: key = iris::Key::KEYPAD_7; break;
        case VK_F20: key = iris::Key::F20; break;
        case VK_NUMPAD8: key = iris::Key::KEYPAD_8; break;
        case VK_NUMPAD9: key = iris::Key::KEYPAD_9; break;
        case VK_F5: key = iris::Key::F5; break;
        case VK_F6: key = iris::Key::F6; break;
        case VK_F7: key = iris::Key::F7; break;
        case VK_F3: key = iris::Key::F3; break;
        case VK_F8: key = iris::Key::F8; break;
        case VK_F9: key = iris::Key::F9; break;
        case VK_F11: key = iris::Key::F11; break;
        case VK_F13: key = iris::Key::F13; break;
        case VK_F16: key = iris::Key::F16; break;
        case VK_F14: key = iris::Key::F14; break;
        case VK_F10: key = iris::Key::F10; break;
        case VK_F12: key = iris::Key::F12; break;
        case VK_F15: key = iris::Key::F15; break;
        case VK_HELP: key = iris::Key::HELP; break;
        case VK_HOME: key = iris::Key::HOME; break;
        case VK_F4: key = iris::Key::F4; break;
        case VK_END: key = iris::Key::END; break;
        case VK_F2: key = iris::Key::F2; break;
        case VK_F1: key = iris::Key::F1; break;
        case VK_LEFT: key = iris::Key::LEFT_ARROW; break;
        case VK_RIGHT: key = iris::Key::RIGHT_ARROW; break;
        case VK_DOWN: key = iris::Key::DOWN_ARROW; break;
        case VK_UP: key = iris::Key::UP_ARROW; break;
        default: key = iris::Key::UNKNOWN;
    }

    return key;
}

/**
 * Callback function for windows events.
 *
 * See:
 * https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)
 * for details
 */
LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
        case WM_CLOSE: event_queue.emplace(iris::QuitEvent{}); break;
        case WM_KEYDOWN:
            event_queue.emplace(iris::KeyboardEvent{
                windows_key_to_engine_Key(wParam), iris::KeyState::DOWN});
            break;
        case WM_KEYUP:
            event_queue.emplace(iris::KeyboardEvent{
                windows_key_to_engine_Key(wParam), iris::KeyState::UP});
            break;
        case WM_INPUT:
        {
            UINT dwSize = sizeof(RAWINPUT);
            BYTE lpb[sizeof(RAWINPUT)];

            ::GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                lpb,
                &dwSize,
                sizeof(RAWINPUTHEADER));

            RAWINPUT raw = {0};
            std::memcpy(&raw, lpb, sizeof(raw));

            if (raw.header.dwType == RIM_TYPEMOUSE)
            {
                // get mouse delta from raw input data
                int x = raw.data.mouse.lLastX;
                int y = raw.data.mouse.lLastY;

                event_queue.emplace(iris::MouseEvent{
                    static_cast<float>(x), static_cast<float>(y)});
            }
            break;
        }
        default: result = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return result;
}

}

namespace iris
{

Win32Window::Win32Window(std::uint32_t width, std::uint32_t height)
    : Window(width, height)
    , window_()
    , dc_()
    , wc_()
{
    // ensure process is aware of high dpi monitors
    ensure(
        ::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK,
        "could not set process dpi awareness");

    const auto instance = ::GetModuleHandleA(NULL);

    // create window class
    wc_ = {};
    wc_.lpfnWndProc = window_proc;
    wc_.hInstance = instance;
    wc_.lpszClassName = "window class";
    wc_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    ensure(::RegisterClassA(&wc_) != 0, "could not register class");

    // create RECT for specified window size
    RECT rect = {0};
    rect.right = static_cast<int>(width_);
    rect.bottom = static_cast<int>(height_);

    ensure(
        ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0,
        "could not resize window");

    // create window, we will resize it after for current dpi
    window_ = {
        CreateWindowExA(
            0,
            wc_.lpszClassName,
            "iris",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            NULL,
            NULL,
            wc_.hInstance,
            NULL),
        ::DestroyWindow};
    ensure(window_, "could not create window");

    const auto scale = screen_scale();

    // ensure window size is correctly scaled for current dpi
    ensure(
        ::SetWindowPos(
            window_,
            window_,
            0,
            0,
            width_ * scale,
            height_ * scale,
            SWP_NOZORDER | SWP_NOACTIVATE) != 0,
        "could not set window position");

    dc_ = {::GetDC(window_), [this](HDC dc) { ::ReleaseDC(window_, dc); }};
    ensure(dc_, "could not get dc");

    ::ShowWindow(window_, SW_SHOW);
    ::UpdateWindow(window_);

    // register for raw mouse events
    RAWINPUTDEVICE rid;
    rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid.usUsage = HID_USAGE_GENERIC_MOUSE;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = window_;

    ensure(
        ::RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE,
        "could not register raw input device");

    // ensure mouse visibility reference count is 0 (mouse is hidden)
    while (::ShowCursor(FALSE) >= 0)
    {
    }
}

std::uint32_t Win32Window::screen_scale() const
{
    const auto dpi = ::GetDpiForWindow(window_);

    return static_cast<std::uint32_t>(
        std::ceil(static_cast<float>(dpi) / 96.0f));
}

std::optional<Event> Win32Window::pump_event()
{
    // non-blocking loop to drain all available windows messages
    MSG message = {0};
    while (::PeekMessageA(&message, NULL, 0, 0, PM_REMOVE) != 0)
    {
        ::TranslateMessage(&message);
        ::DispatchMessageA(&message);
    }

    std::optional<Event> event;

    // get next engine event if one exists
    if (!event_queue.empty())
    {
        event = event_queue.front();
        event_queue.pop();
    }

    return event;
}

HDC Win32Window::device_context() const
{
    return dc_;
}

}
