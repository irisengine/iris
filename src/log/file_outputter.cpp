////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "log/file_outputter.h"

#include <string>

#include "core/error_handling.h"

namespace iris
{

FileOutputter::FileOutputter(const std::string &filename)
    : file_(filename, std::ios::out | std::ios::app)
{
    // better check all of these
    ensure(
        file_.is_open() && !file_.bad() && file_.good() && !file_.fail(),
        "failed to open log file");
}

void FileOutputter::output(const std::string &log)
{
    file_ << log << std::endl;
}

}
