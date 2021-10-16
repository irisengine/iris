////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <fstream>
#include <string>

#include "log/outputter.h"

namespace iris
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
