#pragma once

#include <string>

#include "log/log_level.hpp"

namespace eng
{

/**
 * Interface for a formatter, a class which takes log message details and
 * formats them into a string.
 */
class Formatter
{
    public:

        /** Default */
        virtual ~Formatter() = default;

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
        virtual std::string format(
            const LogLevel level,
            const std::string &tag,
            const std::string &message,
            const std::string &filename,
            const int line) = 0;
};

}

