#include "gl/opengl.hpp"

#include <string>
#include <string_view>

#include "exception.hpp"

namespace eng::gl
{

void check_opengl_error(std::string_view error_message)
{
    if(const auto error = ::glGetError() ; error != GL_NO_ERROR)
    {
        const auto message = std::string{ error_message } + ": " + std::to_string(error);
        throw exception(message);
    }
}

}

