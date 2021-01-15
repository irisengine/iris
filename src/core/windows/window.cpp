#include "core/window.h"

#include <optional>
#include <queue>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <hidusage.h>

#include "core/auto_release.h"
#include "core/exception.h"
#define DONT_MAKE_GL_FUNCTIONS_EXTERN // get concrete function pointers for all
                                      // opengl functions
#include "events/event.h"
#include "events/quit_event.h"
#include "graphics/gl/opengl.h"
#include "graphics/pipeline.h"
#include "graphics/render_system.h"
#include "graphics/render_target.h"

// additional functions we don't want to make public
HGLRC (*wglCreateContextAttribsARB)(HDC, HGLRC, const int *);
BOOL(*wglChoosePixelFormatARB)
(HDC, const int *, const FLOAT *, UINT, int *, UINT *);

namespace
{

// helper aliases
using AutoWindow = iris::AutoRelease<HWND, NULL>;
using AutoDC = iris::AutoRelease<HDC, NULL>;

// as we have to provide a callback to windows for event data and their is no
// way of passing in custom data we use a global queue to store events
std::queue<iris::Event> event_queue;

/**
 * Helper function to resolve a single opengl function.
 *
 * @param function
 *   Reference to function pointer to resolve.
 *
 * @param name
 *   Name of function.
 */
template <class T>
void resolve_opengl_function(T &function, const std::string &name)
{
    const auto address = ::wglGetProcAddress(name.c_str());
    if (address == NULL)
    {
        throw iris::Exception("could not resolve: " + name);
    }

    function = reinterpret_cast<T>(address);
}

/**
 * Helper function to resolve all opengl functions.
 */
void resolve_global_opengl_functions()
{
    resolve_opengl_function(glDeleteBuffers, "glDeleteBuffers");
    resolve_opengl_function(glUseProgram, "glUseProgram");
    resolve_opengl_function(glBindBuffer, "glBindBuffer");
    resolve_opengl_function(glGenVertexArrays, "glGenVertexArrays");
    resolve_opengl_function(glBindVertexArray, "glBindVertexArray");
    resolve_opengl_function(
        glEnableVertexAttribArray, "glEnableVertexAttribArray");
    resolve_opengl_function(glVertexAttribPointer, "glVertexAttribPointer");
    resolve_opengl_function(glVertexAttribIPointer, "glVertexAttribIPointer");
    resolve_opengl_function(glCreateProgram, "glCreateProgram");
    resolve_opengl_function(glAttachShader, "glAttachShader");
    resolve_opengl_function(glGenBuffers, "glGenBuffers");
    resolve_opengl_function(glBufferData, "glBufferData");
    resolve_opengl_function(glLinkProgram, "glLinkProgram");
    resolve_opengl_function(glGetProgramiv, "glGetProgramiv");
    resolve_opengl_function(glGetProgramInfoLog, "glGetProgramInfoLog");
    resolve_opengl_function(glDeleteProgram, "glDeleteProgram");

    resolve_opengl_function(glGenFramebuffers, "glGenFramebuffers");
    resolve_opengl_function(glBindFramebuffer, "glBindFramebuffer");
    resolve_opengl_function(glFramebufferTexture2D, "glFramebufferTexture2D");
    resolve_opengl_function(
        glCheckFramebufferStatus, "glCheckFramebufferStatus");
    resolve_opengl_function(glDeleteFramebuffers, "glDeleteFramebuffers");
    resolve_opengl_function(glGetUniformLocation, "glGetUniformLocation");
    resolve_opengl_function(glUniformMatrix4fv, "glUniformMatrix4fv");
    resolve_opengl_function(glUniform3f, "glUniform3f");
    resolve_opengl_function(glActiveTexture, "glActiveTexture");
    resolve_opengl_function(glUniform1i, "glUniform1i");
    resolve_opengl_function(glBlitFramebuffer, "glBlitFramebuffer");
    resolve_opengl_function(glCreateShader, "glCreateShader");
    resolve_opengl_function(glShaderSource, "glShaderSource");
    resolve_opengl_function(glCompileShader, "glCompileShader");
    resolve_opengl_function(glGetShaderiv, "glGetShaderiv");
    resolve_opengl_function(glGetShaderInfoLog, "glGetShaderInfoLog");
    resolve_opengl_function(glDeleteShader, "glDeleteShader");
    resolve_opengl_function(glGenerateMipmap, "glGenerateMipmap");
}

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

                // mouse has moved - so move it back
                // this locks the mouse to the window

                RECT rect = {0};
                if (::GetWindowRect(hWnd, &rect) == 0)
                {
                    throw iris::Exception("could not get rect");
                }

                if (::ClipCursor(&rect) == 0)
                {
                    throw iris::Exception("could not clip cursor");
                }

                if (::SetCursorPos(
                        (rect.right - rect.left) / 2,
                        (rect.bottom - rect.top) / 2) == 0)
                {
                    throw iris::Exception("could not set cursor");
                }
            }
            break;
        }
        default: result = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return result;
}

/**
 * Helper function to resolve the wgl functions needed to setup an opengl
 * window.
 *
 * We cannot create a win32 opengl window without certain wgl functions, but
 * they cannot be resolved without a window.
 *
 * To get around this catch-22 we create a dummy window, resole the functions
 * then destroy the window. We are then free to setup a proper opengl window.
 *
 * @param instance
 *   A handle to the instance of the module to be associated with the window.
 */
void resolve_wgl_functions(HINSTANCE instance)
{
    // dummy window class
    WNDCLASSA wc = {};
    wc.lpfnWndProc = ::DefWindowProcA;
    wc.hInstance = instance;
    wc.lpszClassName = "dummy window class";
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    if (::RegisterClassA(&wc) == 0)
    {
        throw iris::Exception("could not register class");
    }

    // create dummy window
    const AutoWindow dummy_window = {
        CreateWindowExA(
            0,
            wc.lpszClassName,
            "dummy window",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            wc.hInstance,
            0),
        ::DestroyWindow};

    if (!dummy_window)
    {
        throw iris::Exception("could not create window");
    }

    AutoDC dc = {::GetDC(dummy_window), [&dummy_window](HDC dc) {
                     ::ReleaseDC(dummy_window, dc);
                 }};
    if (!dc)
    {
        throw iris::Exception("could not get dc");
    }

    // pixel format descriptor for dummy window
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    const auto pixel_format = ::ChoosePixelFormat(dc, &pfd);
    if (pixel_format == 0)
    {
        throw iris::Exception("could not choose pixel format");
    }

    if (::SetPixelFormat(dc, pixel_format, &pfd) == FALSE)
    {
        throw iris::Exception("could not set pixel format");
    }

    // get a wgl context
    const iris::AutoRelease<HGLRC, NULL> context = {
        ::wglCreateContext(dc), ::wglDeleteContext};
    if (!context)
    {
        throw iris::Exception("could not create gl context");
    }

    if (::wglMakeCurrent(dc, context) == FALSE)
    {
        throw iris::Exception("could not make current context");
    }

    // resolve our needed functions
    resolve_opengl_function(wglChoosePixelFormatARB, "wglChoosePixelFormatARB");
    resolve_opengl_function(
        wglCreateContextAttribsARB, "wglCreateContextAttribsARB");

    ::wglMakeCurrent(dc, 0);
}

/**
 * Initialise opengl for a window.
 *
 * @param dc
 *   Device context for window.
 */
void init_opengl(HDC dc)
{
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,
        GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,
        GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,
        GL_TRUE,
        WGL_ACCELERATION_ARB,
        WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,
        WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,
        32,
        WGL_DEPTH_BITS_ARB,
        24,
        WGL_STENCIL_BITS_ARB,
        8,
        0};

    int pixel_format = 0;
    UINT num_formats = 0u;
    ::wglChoosePixelFormatARB(
        dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);

    if (num_formats == 0)
    {
        throw iris::Exception("could not choose pixel format");
    }

    // set pixel format

    PIXELFORMATDESCRIPTOR pfd;
    if (::DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd) == 0)
    {
        throw iris::Exception("could not describe pixel format");
    }

    if (::SetPixelFormat(dc, pixel_format, &pfd) == FALSE)
    {
        throw iris::Exception("could not set pixel format");
    }

    // opengl 3.3
    int gl_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        3,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        3,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    const auto context = ::wglCreateContextAttribsARB(dc, 0, gl_attribs);
    if (context == NULL)
    {
        throw iris::Exception("could not create gl context");
    }

    if (!::wglMakeCurrent(dc, context))
    {
        throw iris::Exception("could not set make current context");
    }
}
}

namespace iris
{

struct Window::implementation
{
    AutoWindow window;
    AutoDC dc;
};

Window::Window(float width, float height)
    : width_(width)
    , height_(height)
    , render_system_(nullptr)
    , screen_target_(nullptr)
    , impl_(std::make_unique<implementation>())
{
    const auto instance = ::GetModuleHandleA(NULL);

    // real window class
    WNDCLASSA wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = "window class";
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    if (::RegisterClassA(&wc) == 0)
    {
        throw Exception("could not register class");
    }

    // create RECT for specified window size
    RECT rect = {0};
    rect.right = static_cast<int>(width_);
    rect.bottom = static_cast<int>(height_);

    if (::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) == 0)
    {
        throw Exception("could not resize window");
    }

    // create window
    impl_->window = {
        CreateWindowExA(
            0,
            wc.lpszClassName,
            "iris",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            NULL,
            NULL,
            wc.hInstance,
            NULL),
        ::DestroyWindow};
    if (!impl_->window)
    {
        throw Exception("could not create window");
    }

    impl_->dc = {::GetDC(impl_->window), [this](HDC dc) {
                     ::ReleaseDC(impl_->window, dc);
                 }};
    if (!impl_->dc)
    {
        throw iris::Exception("could not get dc");
    }

    // initialise opengl
    resolve_wgl_functions(wc.hInstance);
    init_opengl(impl_->dc);

    ::ShowWindow(impl_->window, SW_SHOW);
    ::UpdateWindow(impl_->window);

    // we can now resolve all our opengl functions
    resolve_global_opengl_functions();

    // register for raw mouse events
    RAWINPUTDEVICE rid;
    rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid.usUsage = HID_USAGE_GENERIC_MOUSE;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = impl_->window;

    if (::RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
    {
        throw Exception("could not register raw input device");
    }

    // ensure mouse visibility reference count is 0 (mouse is hidden)
    while (::ShowCursor(FALSE) >= 0)
    {
    }

    screen_target_ = std::make_unique<RenderTarget>(
        static_cast<std::uint32_t>(width_),
        static_cast<std::uint32_t>(height_));
    render_system_ =
        std::make_unique<RenderSystem>(width_, height_, screen_target_.get());
}

Window::~Window() = default;

void Window::render(const Pipeline &pipeline) const
{
    render_system_->render(pipeline);

    ::SwapBuffers(impl_->dc);
}

std::optional<Event> Window::pump_event()
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

float Window::width() const
{
    return width_;
}

float Window::height() const
{
    return height_;
}

}
