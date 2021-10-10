////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "log/formatter.h"
#include "log/log_level.h"

namespace iris
{

/**
 * Implementation of Formatter which formats log message details as:
 *   L T [t] F:L | M
 *
 *   Where:
 *     L : First letter of log level
 *     T : Time stamp
 *     t : Tag
 *     F : Filename
 *     L : Line number
 *     M : message
 *
 *  Example:
 *    D 17:38:28 [camera] camera.cpp:58 | x: -4.37114e-08 y: 0 z: -1
 */
class BasicFormatter : public Formatter
{
  public:
    /** Default */
    ~BasicFormatter() override = default;

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
};

}
