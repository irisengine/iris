#pragma once

#include <cstdint>
#include <iostream>
#include <iosfwd>

namespace eng
{

/**
 * Enumeration of log levels.
 */
enum class log_level : std::uint32_t
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

/**
 * Helper function to write a string representation of a log_level enum to a
 * stream.
 *
 * @param out
 *   Stream to write to.
 *
 * @param level
 *   Log level to write.
 *
 * @returns
 *   Reference to input stream.
 */
inline std::ostream& operator<<(std::ostream &out, const log_level level) noexcept
{
    switch(level)
    {
        case log_level::DEBUG:
            out << "DEBUG";
            break;
        case log_level::INFO:
            out << "INFO";
            break;
        case log_level::WARN:
            out << "WARN";
            break;
        case log_level::ERROR:
            out << "ERROR";
            break;
        default:
            out << "UNKNOWN";
            break;
    }

    return out;
}

}

