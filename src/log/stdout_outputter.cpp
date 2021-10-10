////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "log/stdout_outputter.h"

#include <iostream>
#include <string>

namespace iris
{

void StdoutFormatter::output(const std::string &log)
{
    std::cout << log << std::endl;
}

}
