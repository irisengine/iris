#pragma once

#include <string>

#include "formatter.hpp"
#include "log_level.hpp"

namespace eng
{

/**
 * Implementation of formatter which formats log message details as:
 *   L T [t] F:L | M
 *
 *   Where:
 *     L : First letter of log level
 *     T : Time stamp
 *     t : Tag
 *     F : Filename
 *     L : Line number
 *     M : message
 *
 *  Example:
 *    D 17:38:28 [camera] camera.cpp:58 | x: -4.37114e-08 y: 0 z: -1
 */
class basic_formatter final : public formatter
{
    public:

        /** Default */
        basic_formatter() = default;
        ~basic_formatter() override = default;
        basic_formatter(const basic_formatter&) = default;
        basic_formatter& operator=(const basic_formatter&) = default;
        basic_formatter(basic_formatter&&) = default;
        basic_formatter& operator=(basic_formatter&&) = default;

        /**
         * Format the supplied log details into a string.
         *
         * @param level
         *   Log level.
         *
         * @param tag
         *   Tag for log message.
         *
         * @param message
         *   Log message.
         *
         * @param filename
         *   Name of the file logging the message.
         *
         * @param line
         *   Line of the log call in the file.
         */
        std::string format(
            const log_level level,
            const std::string &tag,
            const std::string &message,
            const std::string &filename,
            const int line) override;

};

}

