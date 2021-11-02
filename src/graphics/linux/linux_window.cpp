////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/linux/linux_window.h"

#include <cmath>
#include <iostream>
#include <optional>
#include <queue>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>

#include "core/auto_release.h"
#include "core/error_handling.h"
#include "events/event.h"
#include "events/quit_event.h"
#define DONT_MAKE_GL_FUNCTIONS_EXTERN // get concrete function pointers for all
                                      // opengl functions
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_renderer.h"

namespace
{

class ScopedErrorHandler
{
  public:
    ScopedErrorHandler()
        : old_handler_(nullptr)
    {
        old_handler_ = ::XSetErrorHandler(error_handler);
    }

    ~ScopedErrorHandler()
    {
        ::XSetErrorHandler(old_handler_);
    }

    operator bool() const
    {
        return error;
    }

    void reset()
    {
        error = false;
    }

  private:
    static bool error;

    static int error_handler(Display *, XErrorEvent *)
    {
        error = true;
        return 0;
    }

    int (*old_handler_)(Display *, XErrorEvent *);
};

bool ScopedErrorHandler::error = false;

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
    const auto address = ::glXGetProcAddressARB(reinterpret_cast<const GLubyte *>(name.c_str()));
    iris::ensure(address != NULL, "could not resolve: " + name);

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
    resolve_opengl_function(glDeleteVertexArrays, "glDeleteVertexArrays");
    resolve_opengl_function(glBindVertexArray, "glBindVertexArray");
    resolve_opengl_function(glEnableVertexAttribArray, "glEnableVertexAttribArray");
    resolve_opengl_function(glVertexAttribPointer, "glVertexAttribPointer");
    resolve_opengl_function(glVertexAttribIPointer, "glVertexAttribIPointer");
    resolve_opengl_function(glCreateProgram, "glCreateProgram");
    resolve_opengl_function(glAttachShader, "glAttachShader");
    resolve_opengl_function(glGenBuffers, "glGenBuffers");
    resolve_opengl_function(glBufferData, "glBufferData");
    resolve_opengl_function(glBufferSubData, "glBufferSubData");
    resolve_opengl_function(glLinkProgram, "glLinkProgram");
    resolve_opengl_function(glGetProgramiv, "glGetProgramiv");
    resolve_opengl_function(glGetProgramInfoLog, "glGetProgramInfoLog");
    resolve_opengl_function(glDeleteProgram, "glDeleteProgram");
    resolve_opengl_function(glGenFramebuffers, "glGenFramebuffers");
    resolve_opengl_function(glBindFramebuffer, "glBindFramebuffer");
    resolve_opengl_function(glFramebufferTexture2D, "glFramebufferTexture2D");
    resolve_opengl_function(glCheckFramebufferStatus, "glCheckFramebufferStatus");
    resolve_opengl_function(glDeleteFramebuffers, "glDeleteFramebuffers");
    resolve_opengl_function(glGetUniformLocation, "glGetUniformLocation");
    resolve_opengl_function(glUniformMatrix4fv, "glUniformMatrix4fv");
    resolve_opengl_function(glUniform3f, "glUniform3f");
    resolve_opengl_function(glUniform1fv, "glUniform1fv");
    resolve_opengl_function(glUniform4fv, "glUniform4fv");
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
 * Helper function to convert an X11 key code to an engine key type.
 *
 * @param key_code
 *   X11 key code.
 *
 * @returns
 *   Engine Key.
 */
iris::Key x11_key_to_engine_key(unsigned int key_code)
{
    iris::Key key;

    switch (key_code)
    {
        case XK_0: key = iris::Key::NUM_0; break;
        case XK_1: key = iris::Key::NUM_1; break;
        case XK_2: key = iris::Key::NUM_2; break;
        case XK_3: key = iris::Key::NUM_3; break;
        case XK_4: key = iris::Key::NUM_4; break;
        case XK_5: key = iris::Key::NUM_5; break;
        case XK_6: key = iris::Key::NUM_6; break;
        case XK_7: key = iris::Key::NUM_7; break;
        case XK_8: key = iris::Key::NUM_8; break;
        case XK_9: key = iris::Key::NUM_9; break;
        case XK_a: key = iris::Key::A; break;
        case XK_b: key = iris::Key::B; break;
        case XK_c: key = iris::Key::C; break;
        case XK_d: key = iris::Key::D; break;
        case XK_e: key = iris::Key::E; break;
        case XK_f: key = iris::Key::F; break;
        case XK_g: key = iris::Key::G; break;
        case XK_h: key = iris::Key::H; break;
        case XK_i: key = iris::Key::I; break;
        case XK_j: key = iris::Key::J; break;
        case XK_k: key = iris::Key::K; break;
        case XK_l: key = iris::Key::L; break;
        case XK_m: key = iris::Key::M; break;
        case XK_n: key = iris::Key::N; break;
        case XK_o: key = iris::Key::O; break;
        case XK_p: key = iris::Key::P; break;
        case XK_q: key = iris::Key::Q; break;
        case XK_r: key = iris::Key::R; break;
        case XK_s: key = iris::Key::S; break;
        case XK_t: key = iris::Key::T; break;
        case XK_u: key = iris::Key::U; break;
        case XK_v: key = iris::Key::V; break;
        case XK_w: key = iris::Key::W; break;
        case XK_x: key = iris::Key::X; break;
        case XK_y: key = iris::Key::Y; break;
        case XK_z: key = iris::Key::Z; break;
        case XK_Tab: key = iris::Key::TAB; break;
        case XK_space: key = iris::Key::SPACE; break;
        case XK_Escape: key = iris::Key::ESCAPE; break;
        case XK_Shift_L: key = iris::Key::SHIFT; break;
        case XK_Shift_R: key = iris::Key::RIGHT_SHIFT; break;
        case XK_F17: key = iris::Key::F17; break;
        case XK_period: key = iris::Key::KEYPAD_DECIMAL; break;
        case XK_multiply: key = iris::Key::KEYPAD_MULTIPLY; break;
        case XK_plus: key = iris::Key::KEYPAD_PLUS; break;
        case XK_division: key = iris::Key::KEYPAD_DIVIDE; break;
        case XK_minus: key = iris::Key::KEYPAD_MINUS; break;
        case XK_F18: key = iris::Key::F18; break;
        case XK_F19: key = iris::Key::F19; break;
        case XK_KP_0: key = iris::Key::KEYPAD_0; break;
        case XK_KP_1: key = iris::Key::KEYPAD_1; break;
        case XK_KP_2: key = iris::Key::KEYPAD_2; break;
        case XK_KP_3: key = iris::Key::KEYPAD_3; break;
        case XK_KP_4: key = iris::Key::KEYPAD_4; break;
        case XK_KP_5: key = iris::Key::KEYPAD_5; break;
        case XK_KP_6: key = iris::Key::KEYPAD_6; break;
        case XK_KP_7: key = iris::Key::KEYPAD_7; break;
        case XK_F20: key = iris::Key::F20; break;
        case XK_KP_8: key = iris::Key::KEYPAD_8; break;
        case XK_KP_9: key = iris::Key::KEYPAD_9; break;
        case XK_F5: key = iris::Key::F5; break;
        case XK_F6: key = iris::Key::F6; break;
        case XK_F7: key = iris::Key::F7; break;
        case XK_F3: key = iris::Key::F3; break;
        case XK_F8: key = iris::Key::F8; break;
        case XK_F9: key = iris::Key::F9; break;
        case XK_F11: key = iris::Key::F11; break;
        case XK_F13: key = iris::Key::F13; break;
        case XK_F16: key = iris::Key::F16; break;
        case XK_F14: key = iris::Key::F14; break;
        case XK_F10: key = iris::Key::F10; break;
        case XK_F12: key = iris::Key::F12; break;
        case XK_F15: key = iris::Key::F15; break;
        case XK_F4: key = iris::Key::F4; break;
        case XK_F2: key = iris::Key::F2; break;
        case XK_F1: key = iris::Key::F1; break;
        case XK_Left: key = iris::Key::LEFT_ARROW; break;
        case XK_Right: key = iris::Key::RIGHT_ARROW; break;
        case XK_Down: key = iris::Key::DOWN_ARROW; break;
        case XK_Up: key = iris::Key::UP_ARROW; break;
        default: key = iris::Key::UNKNOWN;
    }

    return key;
}

}

//
///**
// * Callback function for windows events.
// *
// * See:
// * https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)
// * for details
// */
// LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    LRESULT result = 0;
//
//    switch (uMsg)
//    {
//        case WM_CLOSE: event_queue.emplace(iris::QuitEvent{}); break;
//        case WM_KEYDOWN:
//            event_queue.emplace(iris::KeyboardEvent{windows_key_to_engine_Key(wParam), iris::KeyState::DOWN});
//            break;
//        case WM_KEYUP:
//            event_queue.emplace(iris::KeyboardEvent{windows_key_to_engine_Key(wParam), iris::KeyState::UP});
//            break;
//        case WM_INPUT:
//        {
//            UINT dwSize = sizeof(RAWINPUT);
//            BYTE lpb[sizeof(RAWINPUT)];
//
//            ::GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
//
//            RAWINPUT raw = {0};
//            std::memcpy(&raw, lpb, sizeof(raw));
//
//            if (raw.header.dwType == RIM_TYPEMOUSE)
//            {
//                // get mouse delta from raw input data
//                int x = raw.data.mouse.lLastX;
//                int y = raw.data.mouse.lLastY;
//
//                event_queue.emplace(iris::MouseEvent{static_cast<float>(x), static_cast<float>(y)});
//            }
//            break;
//        }
//        default: result = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
//    }
//
//    return result;
//}
//
//}

namespace iris
{

LinuxWindow::LinuxWindow(std::uint32_t width, std::uint32_t height)
    : Window(width, height)
{
    display_ = {::XOpenDisplay(nullptr), ::XCloseDisplay};
    ensure(display_, "could not open display");

    static const int visual_attribs[] = {
        GLX_X_RENDERABLE,
        True,
        GLX_DRAWABLE_TYPE,
        GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,
        GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,
        GLX_TRUE_COLOR,
        GLX_RED_SIZE,
        8,
        GLX_GREEN_SIZE,
        8,
        GLX_BLUE_SIZE,
        8,
        GLX_ALPHA_SIZE,
        8,
        GLX_DEPTH_SIZE,
        24,
        GLX_STENCIL_SIZE,
        8,
        GLX_DOUBLEBUFFER,
        True,
        None};

    int glx_major = 0;
    int glx_minor = 0;

    ensure(::glXQueryVersion(display_, &glx_major, &glx_minor) == True, "could not query extension version");
    ensure((glx_major > 1) || ((glx_major == 1) && (glx_minor >= 3)), "incompatible glx version");

    int fb_count = 0;

    AutoRelease<GLXFBConfig *, nullptr> fb_config = {
        ::glXChooseFBConfig(display_, DefaultScreen(display_.get()), visual_attribs, &fb_count), ::XFree};
    ensure(fb_config && (fb_count > 0), "could not get framebuffer config");

    int best_fb_config = -1;
    int best_num_samples = -1;

    for (auto i = 0; i < fb_count; ++i)
    {
        AutoRelease<XVisualInfo *, nullptr> info = {::glXGetVisualFromFBConfig(display_, fb_config[i]), ::XFree};
        if (info)
        {
            int sample_buffer = 0;
            int samples = 0;

            if (::glXGetFBConfigAttrib(display_, fb_config[i], GLX_SAMPLE_BUFFERS, &sample_buffer) == Success)
            {
                if (::glXGetFBConfigAttrib(display_, fb_config[i], GLX_SAMPLES, &samples) == Success)
                {
                    if (samples > best_num_samples)
                    {
                        best_num_samples = samples;
                        best_fb_config = i;
                    }
                }
            }
        }
    }

    auto config = fb_config[best_fb_config];

    AutoRelease<XVisualInfo *, nullptr> visual_info = {::glXGetVisualFromFBConfig(display_, config), ::XFree};
    ensure(visual_info, "could not get visual info");

    AutoRelease<Colormap, 0> colour_map = {
        ::XCreateColormap(
            display_, RootWindow(display_.get(), visual_info.get()->screen), visual_info.get()->visual, AllocNone),
        [this](Colormap colour_map) { ::XFreeColormap(display_, colour_map); }};

    XSetWindowAttributes attributes{0};
    attributes.colormap = colour_map;
    attributes.background_pixmap = 0;
    attributes.border_pixel = 0;
    attributes.event_mask = StructureNotifyMask;

    window_ = {
        ::XCreateWindow(
            display_,
            RootWindow(display_.get(), visual_info.get()->screen),
            0,
            0,
            width_,
            height_,
            0,
            visual_info.get()->depth,
            InputOutput,
            visual_info.get()->visual,
            CWBorderPixel | CWColormap | CWEventMask,
            &attributes),
        [this](::Window window) { ::XDestroyWindow(display_, window); }};
    ensure(window_, "could not create widnow");

    ::XSelectInput(
        display_,
        window_,
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);

    Atom wmDeleteMessage = XInternAtom(display_, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display_, window_, &wmDeleteMessage, 1);

    ::XMapWindow(display_, window_);

    {
        ScopedErrorHandler error_handler;

        auto *glXCreateContextAttribsARB =
            reinterpret_cast<GLXContext (*)(Display *, GLXFBConfig, GLXContext, Bool, const int *)>(
                ::glXGetProcAddressARB(reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")));
        ensure(glXCreateContextAttribsARB != nullptr, "unsupported extension");

        resolve_global_opengl_functions();

        const int context_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3, None};

        context_ = {glXCreateContextAttribsARB(display_, config, 0, True, context_attribs), [this](GLXContext context) {
                        ::glXDestroyContext(display_, context);
                    }};

        ::XSync(display_, False);
        ensure(context_ && (error_handler == false), "could not create context");
    }

    ensure(::glXMakeCurrent(display_, window_, context_) == True, "could not make context current");

    renderer_ = std::make_unique<OpenGLRenderer>(width_, height_);

    const auto scale = screen_scale();

    XWindowChanges changes{0};
    changes.width = width_ * scale;
    changes.height = height_ * scale;

    ::XConfigureWindow(display_, window_, CWWidth | CWHeight, &changes);

    ::XSetLocaleModifiers("");

    ::XFixesHideCursor(display_, window_);
    ::XFlush(display_);

    ::XWarpPointer(display_, None, window_, 0, 0, 0, 0, changes.width / 2, changes.height / 2);
}

std::uint32_t LinuxWindow::screen_scale() const
{
    const auto dpi = ((double)DisplayWidth(display_.get(), 0)) / (((double)DisplayWidthMM(display_.get(), 0)) / 25.4);
    return static_cast<std::uint32_t>(std::floor(static_cast<float>(dpi) / 96.0f));
}

std::optional<Event> LinuxWindow::pump_event()
{
    XEvent event{0};

    while (::XCheckWindowEvent(
               display_,
               window_,
               KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask,
               &event) == True)
    {
        if (event.type == KeyPress)
        {
            events_.emplace(KeyboardEvent{x11_key_to_engine_key(::XLookupKeysym(&event.xkey, 0)), KeyState::DOWN});
        }
        else if (event.type == KeyRelease)
        {
            events_.emplace(KeyboardEvent{x11_key_to_engine_key(::XLookupKeysym(&event.xkey, 0)), KeyState::UP});
        }
        else if (event.type == MotionNotify)
        {
            static auto x = event.xmotion.x;
            static auto y = event.xmotion.y;

            events_.emplace(
                MouseEvent{static_cast<float>(event.xmotion.x - x), static_cast<float>(event.xmotion.y - y)});

            x = event.xmotion.x;
            y = event.xmotion.y;
        }
    }

    std::optional<Event> next_event{};

    if (!events_.empty())
    {
        next_event = events_.front();
        events_.pop();
    }

    return next_event;
}

Display *LinuxWindow::display() const
{
    return display_.get();
}

::Window LinuxWindow::window() const
{
    return window_.get();
}

}
