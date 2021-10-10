////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "log/basic_formatter.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "log/log_level.h"

namespace
{

/**
 * Helper function to extract just the filename from a full path.
 *
 * @param filename
 *   Filename to extract from.
 *
 * @returns
 *   Filename from supplied string.
 */
std::string format_filename(const std::string &filename)
{
    // find last occurrence of file separator
    const auto index = filename.rfind(std::filesystem::path::preferred_separator);

    return std::string{filename.substr(index + 1)};
}

/**
 * Convert log level to string and get first character.
 *
 * @param level
 *   Log level to get first character of.
 *
 * @returns
 *   First character of log level.
 */
char first_char_of_level(const iris::LogLevel level)
{
    std::stringstream strm{};
    strm << level;

    const auto str = strm.str();

    return !str.empty() ? str.front() : 'U';
}

}

namespace iris
{

std::string BasicFormatter::format(
    const LogLevel level,
    const std::string &tag,
    const std::string &message,
    const std::string &filename,
    const int line)
{
    const auto now = std::chrono::system_clock::now();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

    std::stringstream strm{};

    strm << first_char_of_level(level) << " " << seconds.count() << " [" << tag << "] " << format_filename(filename)
         << ":" << line << " | " << message;
    return strm.str();
}

}
