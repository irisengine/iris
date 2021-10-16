////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdexcept>
#include <string>

namespace iris
{

/**
 * The general exception type for the engine.
 */
class Exception : public std::runtime_error
{
  public:
    /**
     * Construct a new exception.
     *
     * @param what
     *   Exception message.
     */
    explicit Exception(const std::string &what);
};

}
