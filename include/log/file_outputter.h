#pragma once

#include <string>
#include <fstream>

#include "log/outputter.h"

namespace eng
{

/**
 * Implementation of Outputter which writes log messages to filet.
 */
class FileOutputter : public Outputter
{
    public:

        /** Default */
        ~FileOutputter() override = default;

        /**
         * Construct a new file_outputter.
         *
         * @param filename
         *   Name of log file to write to.
         */
        explicit FileOutputter(const std::string &filename);

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

