#pragma once

#include <string>

#include "basic_formatter.hpp"
#include "formatter.hpp"
#include "log_level.hpp"

namespace eng
{

/**
 * Implementation of formatter which applies colour to basic formatter.
 */
class colour_formatter final : public formatter
{
    public:

        /** Default */
        colour_formatter() = default;
        ~colour_formatter() override = default;
        colour_formatter(const colour_formatter&) = default;
        colour_formatter& operator=(const colour_formatter&) = default;
        colour_formatter(colour_formatter&&) = default;
        colour_formatter& operator=(colour_formatter&&) = default;

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

    private:

        basic_formatter formatter_;

};

}


