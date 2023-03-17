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
#include "events/keyboard_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "events/quit_event.h"
#include "events/scroll_wheel_event.h"
#include "graphics/linux/scoped_error_handler.h"
#include "graphics/window_manager.h"
#define DONT_MAKE_GL_FUNCTIONS_EXTERN // get concrete function pointers for all
                                      // opengl functions
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_renderer.h"

bool iris::ScopedErrorHandler::error = false;

// x-macro argument to resolve functions
#define RESOLVE_FUNCTION(_, NAME, ...) resolve_opengl_function(NAME, #NAME);

namespace
{

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
    FOR_OPENGL_FUNCTIONS(RESOLVE_FUNCTION);
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

namespace iris
{

LinuxWindow::LinuxWindow(
    std::uint32_t width,
    std::uint32_t height,
    WindowManager &window_manager,
    TextureManager &texture_manager,
    MaterialManager &material_manager)
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

    // check we have the minimum version of glx (the opengl extension to x11)
    ensure(::glXQueryVersion(display_, &glx_major, &glx_minor) == True, "could not query extension version");
    ensure((glx_major > 1) || ((glx_major == 1) && (glx_minor >= 3)), "incompatible glx version");

    int fb_count = 0;

    // get all the frame buffer configurations that match our attributes
    AutoRelease<GLXFBConfig *, nullptr> fb_config = {
        ::glXChooseFBConfig(display_, DefaultScreen(display_.get()), visual_attribs, &fb_count), ::XFree};
    ensure(fb_config && (fb_count > 0), "could not get framebuffer config");

    int best_fb_config = -1;
    int best_num_samples = -1;

    // find the config with the most samples
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

    // get the visual info for our chosen config
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

    // create an x11 window
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

    // set which events we want to receive
    ::XSelectInput(
        display_,
        window_,
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);

    // register for close window events
    Atom wmDeleteMessage = XInternAtom(display_, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display_, window_, &wmDeleteMessage, 1);

    ::XMapWindow(display_, window_);

    // check opengl version

    {
        ScopedErrorHandler error_handler;

        // we need to find this function first
        auto *glXCreateContextAttribsARB =
            reinterpret_cast<GLXContext (*)(Display *, GLXFBConfig, GLXContext, Bool, const int *)>(
                ::glXGetProcAddressARB(reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")));
        ensure(glXCreateContextAttribsARB != nullptr, "unsupported extension");

        resolve_global_opengl_functions();

        // want at least opengl 3.3
        const int context_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3, None};

        context_ = {glXCreateContextAttribsARB(display_, config, 0, True, context_attribs), [this](GLXContext context) {
                        ::glXDestroyContext(display_, context);
                    }};

        ::XSync(display_, False);
        ensure(context_ && (error_handler == false), "could not create context");
    }

    ensure(::glXMakeCurrent(display_, window_, context_) == True, "could not make context current");

    renderer_ = std::make_unique<OpenGLRenderer>(window_manager, texture_manager, material_manager, width_, height_);

    const auto scale = screen_scale();

    XWindowChanges changes{0};
    changes.width = width_ * scale;
    changes.height = height_ * scale;

    // resize window based on scale
    ::XConfigureWindow(display_, window_, CWWidth | CWHeight, &changes);

    ::XSetLocaleModifiers("");

    // hide cursor
    ::XFixesHideCursor(display_, window_);
    ::XFlush(display_);

    // move cursor to centre of screen
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
        else if (event.type == ButtonPress)
        {
            if (event.xbutton.button == Button1)
            {
                events_.emplace(MouseButtonEvent{MouseButton::LEFT, MouseButtonState::DOWN});
            }
            else if (event.xbutton.button == Button2)
            {
                events_.emplace(MouseButtonEvent{MouseButton::RIGHT, MouseButtonState::DOWN});
            }
            else if (event.xbutton.button == Button4)
            {
                events_.emplace(ScrollWheelEvent{1.0f});
            }
            else if (event.xbutton.button == Button5)
            {
                events_.emplace(ScrollWheelEvent{-1.0f});
            }
        }
        else if (event.type == ButtonRelease)
        {
            if (event.xbutton.button == Button1)
            {
                events_.emplace(MouseButtonEvent{MouseButton::LEFT, MouseButtonState::UP});
            }
            else if (event.xbutton.button == Button2)
            {
                events_.emplace(MouseButtonEvent{MouseButton::RIGHT, MouseButtonState::UP});
            }
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
