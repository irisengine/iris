#include "gl/opengl.hpp"

#include <stdexcept>
#include <string>
#include <string_view>

namespace eng::gl
{

void check_opengl_error(std::string_view error_message)
{
    if(const auto error = ::glGetError() ; error != GL_NO_ERROR)
    {
        const auto message = std::string{ error_message } + ": " + std::to_string(error);
        throw std::runtime_error(message);
    }
}

}

