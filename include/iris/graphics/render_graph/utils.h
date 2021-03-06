#pragma once

#include <cstddef>
#include <sstream>
#include <string>

namespace iris
{

/**
 * Replace all occurrences of "{}" in a string with an index. This is used for
 * shader generation.
 *
 * @param input
 *   String to modify.
 *
 * @param index
 *   Index to write into input.
 *
 * @returns
 *   Modified string.
 */
std::string replace_index(const std::string &input, std::size_t index);

/**
 * Replace all occurrences of "{}" in a string with one index and all
 * occurrences of "<>" with another. This is used for shader generation.
 *
 * @param input
 *   String to modify.
 *
 * @param index1
 *   Index to write into input.
 *
 * @param index2
 *   Index to write into input.
 *
 * @returns
 *   Modified string.
 */
std::string replace_index(
    const std::string &input,
    std::size_t index1,
    std::size_t index2);

}
