#include "graphics/gl/opengl.h"

#include <string>

#include "core/exception.h"

namespace eng
{

void check_opengl_error(const std::string &error_message)
{
    if(const auto error = ::glGetError() ; error != GL_NO_ERROR)
    {
        const auto message = std::string{ error_message } + ": " + std::to_string(error);
        throw Exception(message);
    }
}

}

