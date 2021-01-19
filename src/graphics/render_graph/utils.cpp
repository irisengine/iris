#include "graphics/render_graph/utils.h"

#include <cstddef>
#include <sstream>
#include <string>

namespace iris
{

std::string replace_index(const std::string &input, std::size_t index)
{
    std::stringstream strm;

    static const std::string delimiter = "{}";
    std::size_t start = 0u;
    auto end = input.find(delimiter);

    while (end != std::string::npos)
    {
        strm << input.substr(start, end - start) << index;
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }

    strm << input.substr(start, end);

    return strm.str();
}

}
