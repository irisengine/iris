#pragma once

#include <string>

#if defined(PLATFORM_MACOS)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
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

