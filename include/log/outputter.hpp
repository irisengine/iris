#pragma once

#include <string>

namespace eng
{


/**
 * Interface for an outputter, a class which writes a log message to some
 * implementation defined medium.
 */
class outputter
{
    public:

        /** Default */
        outputter() = default;
        virtual ~outputter() = default;
        outputter(const outputter&) = default;
        outputter& operator=(const outputter&) = default;
        outputter(outputter&&) = default;
        outputter& operator=(outputter&&) = default;

        /**
         * Output log.
         *
         * @param log
         *   Log message to output.
         */
        virtual void output(const std::string &log) = 0;
};

}

