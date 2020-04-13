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
        virtual ~outputter() = default;

        /**
         * Output log.
         *
         * @param log
         *   Log message to output.
         */
        virtual void output(const std::string &log) = 0;
};

}

