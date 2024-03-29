////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "log/colour_formatter.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "log/log_level.h"

namespace iris
{

std::string ColourFormatter::format(
    const LogLevel level,
    const std::string &tag,
    const std::string &message,
    const std::string &filename,
    const int line)
{
    std::stringstream strm{};

    // apply an ANSI escape sequence to start colour output
    switch (level)
    {
        case LogLevel::DEBUG: strm << "\x1b[35m"; break;
        case LogLevel::INFO: strm << "\x1b[34m"; break;
        case LogLevel::WARN: strm << "\x1b[33m"; break;
        case LogLevel::ERR: strm << "\x1b[31m"; break;
        default: break;
    }

    // write message and reset ANSI escape code
    strm << formatter_.format(level, tag, message, filename, line) << "\x1b[0m";

    return strm.str();
}

}
