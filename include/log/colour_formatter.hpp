#pragma once

#include <string>

#include "log/basic_formatter.hpp"
#include "log/formatter.hpp"
#include "log/log_level.hpp"

namespace eng
{

/**
 * Implementation of Formatter which applies colour to basic formatter.
 */
class ColourFormatter : public Formatter
{
    public:

        /** Default */
        ~ColourFormatter() override = default;

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
            const LogLevel level,
            const std::string &tag,
            const std::string &message,
            const std::string &filename,
            const int line) override;

    private:

        /** Use BasicFormatter for formatting. */
        BasicFormatter formatter_;

};

}


