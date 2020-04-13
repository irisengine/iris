#include "gl/opengl.hpp"

#include <string>

#include "exception.hpp"

namespace eng
{

void check_opengl_error(const std::string &error_message)
{
    if(const auto error = ::glGetError() ; error != GL_NO_ERROR)
    {
        const auto message = std::string{ error_message } + ": " + std::to_string(error);
        throw exception(message);
    }
}

}

