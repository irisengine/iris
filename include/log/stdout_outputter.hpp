#pragma once

#include <string>

#include "outputter.hpp"

namespace eng
{

/**
 * Implementation of outputter which writes log messages to stdout.
 */
class stdout_outputter final : public outputter
{
    public:

        /** Default */
        stdout_outputter() = default;
        ~stdout_outputter() override = default;
        stdout_outputter(const stdout_outputter&) = default;
        stdout_outputter& operator=(const stdout_outputter&) = default;
        stdout_outputter(stdout_outputter&&) = default;
        stdout_outputter& operator=(stdout_outputter&&) = default;

        /**
         * Output log.
         *
         * @param log
         *   Log message to output.
         */
        void output(const std::string &log) override;
};

}

