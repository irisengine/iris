#pragma once

#include <cstdint>
#include <iostream>
#include <iosfwd>

namespace eng
{

/**
 * Enumeration of log levels.
 */
enum class LogLevel : std::uint32_t
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

/**
 * Helper function to write a string representation of a LogLevel enum to a
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
inline std::ostream& operator<<(std::ostream &out, const LogLevel level)
{
    switch(level)
    {
        case LogLevel::DEBUG:
            out << "DEBUG";
            break;
        case LogLevel::INFO:
            out << "INFO";
            break;
        case LogLevel::WARN:
            out << "WARN";
            break;
        case LogLevel::ERROR:
            out << "ERROR";
            break;
        default:
            out << "UNKNOWN";
            break;
    }

    return out;
}

}

