#pragma once

#include <string_view>

#if defined(PLATFORM_OSX)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#error unsupported platform
#endif

namespace eng::gl
{

/**
 * Throws an exception if an OpenGl error has occured.
 *
 * @param error_message
 *   The message to include in the exception.
 */
void check_opengl_error(std::string_view error_message);

}

