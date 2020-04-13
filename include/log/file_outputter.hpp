#pragma once

#include <string>
#include <fstream>

#include "outputter.hpp"

namespace eng
{

/**
 * Implementation of outputter which writes log messages to filet.
 */
class file_outputter : public outputter
{
    public:

        /** Default */
        ~file_outputter() override = default;

        /**
         * Construct a new file_outputter.
         *
         * @param filename
         *   Name of log file to write to.
         */
        explicit file_outputter(const std::string &filename);

        /**
         * Output log.
         *
         * @param log
         *   Log message to output.
         */
        void output(const std::string &log) override;

    private:

        /** File stream to write to. */
        std::ofstream file_;
};

}

