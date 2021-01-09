#pragma once

#include <string>

// platform specific opengl includes

#if defined(PLATFORM_MACOS)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(PLATFORM_WINDOWS)
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
#include "graphics/gl/opengl_windows.h"
#pragma comment(lib, "opengl32.lib")
// clang-format on
#else
#error unsupported platform
#endif

namespace iris
{

/**
 * Throws an exception if an OpenGl error has occured.
 *
 * @param error_message
 *   The message to include in the exception.
 */
void check_opengl_error(const std::string &error_message);

}
