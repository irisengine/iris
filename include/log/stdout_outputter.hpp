#pragma once

#include <string>

#include "log/outputter.hpp"

namespace eng
{

/**
 * Implementation of outputter which writes log messages to stdout.
 */
class stdout_outputter : public outputter
{
    public:

        /** Default */
        ~stdout_outputter() override = default;

        /**
         * Output log.
         *
         * @param log
         *   Log message to output.
         */
        void output(const std::string &log) override;
};

}

