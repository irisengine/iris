////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "log/emoji_formatter.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "log/log_level.h"

namespace iris
{

std::string EmojiFormatter::format(
    const LogLevel level,
    const std::string &tag,
    const std::string &message,
    const std::string &filename,
    const int line)
{
    std::stringstream strm{};

    // apply an emoji to start of output
    // depending on your text editor the emojis below may not display, but they
    // are there!
    switch (level)
    {
        case LogLevel::DEBUG: strm << "🔵 "; break;
        case LogLevel::INFO: strm << "ℹ️ "; break;
        case LogLevel::WARN: strm << "⚠️ "; break;
        case LogLevel::ERR: strm << "❌ "; break;
        default: break;
    }

    // write message
    strm << formatter_.format(level, tag, message, filename, line);

    return strm.str();
}

}
