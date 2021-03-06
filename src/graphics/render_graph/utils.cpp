#include "graphics/render_graph/utils.h"

#include <cstddef>
#include <sstream>
#include <string>

namespace
{

/**
 * Helper function to replace all occurrences of a string with a value.
 *
 * @param input
 *   String to make replacements on.
 *
 * @param find
 *   String to replace.
 *
 * @param replace
 *   Value to replace
 *
 * @returns
 *   Input with strings replaced.
 */
template <class T>
std::string replace(
    const std::string &input,
    const std::string &find,
    T replace)
{
    std::stringstream strm;

    std::size_t start = 0u;
    auto end = input.find(find);

    while (end != std::string::npos)
    {
        strm << input.substr(start, end - start) << replace;
        start = end + find.length();
        end = input.find(find, start);
    }

    strm << input.substr(start, end);

    return strm.str();
}

}

namespace iris
{

std::string replace_index(const std::string &input, std::size_t index)
{
    return replace(input, "{}", index);
}

std::string replace_index(
    const std::string &input,
    std::size_t index1,
    std::size_t index2)
{
    const auto first_pass = replace(input, "{}", index1);
    return replace(first_pass, "<>", index2);
}

}
