////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "log/outputter.h"

namespace iris
{

/**
 * Implementation of Outputter which writes log messages to stdout.
 */
class StdoutFormatter : public Outputter
{
  public:
    /** Default */
    ~StdoutFormatter() override = default;

    /**
     * Output log.
     *
     * @param log
     *   Log message to output.
     */
    void output(const std::string &log) override;
};

}
