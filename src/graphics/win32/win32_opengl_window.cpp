////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/win32/win32_opengl_window.h"

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
#define DONT_MAKE_GL_FUNCTIONS_EXTERN // get concrete function pointers for all
                                      // opengl functions
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_renderer.h"

#pragma comment(lib, "Shcore.lib")

// additional functions we don't want to make public
HGLRC (*wglCreateContextAttribsARB)(HDC, HGLRC, const int *);
BOOL(*wglChoosePixelFormatARB)
(HDC, const int *, const FLOAT *, UINT, int *, UINT *);
BOOL (*wglSwapIntervalEXT)(int);

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
    const auto address = ::wglGetProcAddress(name.c_str());
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
    resolve_opengl_function(
        glEnableVertexAttribArray, "glEnableVertexAttribArray");
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
    resolve_opengl_function(
        glCheckFramebufferStatus, "glCheckFramebufferStatus");
    resolve_opengl_function(glDeleteFramebuffers, "glDeleteFramebuffers");
    resolve_opengl_function(glGetUniformLocation, "glGetUniformLocation");
    resolve_opengl_function(glUniformMatrix4fv, "glUniformMatrix4fv");
    resolve_opengl_function(glUniform3f, "glUniform3f");
    resolve_opengl_function(glUniform1fv, "glUniform1fv");
    resolve_opengl_function(glUniform4fv, "glUniform4fv");
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

    iris::ensure(::RegisterClassA(&wc) != 0, "could not register class");

    // create dummy window
    const iris::Win32OpenGLWindow::AutoWindow dummy_window = {
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

    iris::ensure(dummy_window, "could not create window");

    iris::Win32OpenGLWindow::AutoDC dc = {
        ::GetDC(dummy_window),
        [&dummy_window](HDC dc) { ::ReleaseDC(dummy_window, dc); }};
    iris::ensure(dc, "could not get dc");

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
    iris::ensure(pixel_format != 0, "could not choose pixel format");

    if (::SetPixelFormat(dc, pixel_format, &pfd) == FALSE)
    {
        throw iris::Exception("could not set pixel format");
    }

    // get a wgl context
    const iris::AutoRelease<HGLRC, nullptr> context = {
        ::wglCreateContext(dc), ::wglDeleteContext};
    iris::ensure(context, "could not create gl context");

    iris::ensure(
        ::wglMakeCurrent(dc, context) == TRUE,
        "could not make current context");

    // resolve our needed functions
    resolve_opengl_function(wglChoosePixelFormatARB, "wglChoosePixelFormatARB");
    resolve_opengl_function(
        wglCreateContextAttribsARB, "wglCreateContextAttribsARB");
    resolve_opengl_function(wglSwapIntervalEXT, "wglSwapIntervalEXT");

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

    iris::ensure(num_formats != 0, "could not choose pixel format");

    // set pixel format

    PIXELFORMATDESCRIPTOR pfd;
    iris::ensure(
        ::DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd) != 0,
        "could not describe pixel format");

    iris::ensure(
        ::SetPixelFormat(dc, pixel_format, &pfd) == TRUE,
        "could not set pixel format");

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
    iris::ensure(context != NULL, "could not create gl context");

    iris::ensure(
        ::wglMakeCurrent(dc, context) == TRUE,
        "could not set make current context");

    // disable vsync
    iris::ensure(::wglSwapIntervalEXT(0) == TRUE, "could not disable vsync");
}
}

namespace iris
{

Win32OpenGLWindow::Win32OpenGLWindow(std::uint32_t width, std::uint32_t height)
    : Win32Window(width, height)
{
    // initialise opengl
    resolve_wgl_functions(wc_.hInstance);
    init_opengl(dc_);

    // we can now resolve all our opengl functions
    resolve_global_opengl_functions();

    renderer_ = std::make_unique<OpenGLRenderer>(width_, height_);
}

}
