////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>

// platform specific opengl includes

#if defined(IRIS_PLATFORM_MACOS)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(IRIS_PLATFORM_WIN32)
// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "gl/gl.h"

// in order to avoid duplicating all the opengl function definitions as both
// extern and concrete we can use the EXTERN macro to control its linkage
// by default all functions will be marked as extern (the common case) unless
// this include is prefaced with DONT_MAKE_GL_FUNCTIONS_EXTERN
#if defined(DONT_MAKE_GL_FUNCTIONS_EXTERN)
#define EXTERN
#else
#define EXTERN extern
#endif
#include "graphics/opengl/opengl_windows.h"
#pragma comment(lib, "opengl32.lib")
// clang-format on
#else
#error unsupported platform
#endif

namespace iris
{

/**
 * Throws an exception if an OpenGl error has occurred.
 *
 * @param error_message
 *   The message to include in the exception.
 *
 * @returns
 *   Empty optional if no error occurred, otherwise an error string.
 */
std::optional<std::string> do_check_opengl_error(
    std::string_view error_message);

// create std::function so it can be passed to ensure/expect
static const std::function<std::optional<std::string>(std::string_view)>
    check_opengl_error = do_check_opengl_error;

}
