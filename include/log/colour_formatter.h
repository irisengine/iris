#pragma once

#include <string>

#include "log/basic_formatter.h"
#include "log/formatter.h"
#include "log/log_level.h"

namespace iris
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


