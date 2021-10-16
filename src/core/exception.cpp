////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/exception.h"

#include <stdexcept>
#include <string>

namespace iris
{

Exception::Exception(const std::string &what)
    : std::runtime_error(what)
{
}

}
