#include "graphics/opengl/opengl.h"

#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace iris
{

std::optional<std::string> do_check_opengl_error(std::string_view error_message)
{
    std::optional<std::string> final_message{};

    if (const auto error = ::glGetError(); error != GL_NO_ERROR)
    {
        std::stringstream strm{};
        strm << error_message << " : " << error;
        final_message = strm.str();
    }

    return final_message;
}

}
