#include "log/basic_formatter.hpp"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "log/log_level.hpp"

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
    const auto index = filename.rfind(
        std::filesystem::path::preferred_separator);

    return std::string{ filename.substr(index + 1) };
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
char first_char_of_level(const eng::LogLevel level)
{
    std::stringstream strm{ };
    strm << level;

    const auto str = strm.str();

    return !str.empty()
        ? str.front()
        : 'U';
}

}

namespace eng
{

std::string BasicFormatter::format(
    const LogLevel level,
    const std::string &tag,
    const std::string &message,
    const std::string &filename,
    const int line)
{
    const auto now = std::chrono::system_clock::now();
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream strm{ };

    strm <<
        first_char_of_level(level) << " " <<
        std::put_time(std::localtime(&in_time_t), "%X") <<
        " [" << tag << "] " <<
        format_filename(filename) << ":" <<
        line <<
        " | " << message;

    return strm.str();
}

}

