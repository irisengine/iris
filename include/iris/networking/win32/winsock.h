////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace iris
{
/**
 * RAII class to initialise and cleanup winsock.
 */
class Winsock
{
  public:
    /**
     * Initialise winsock.
     */
    Winsock();

    /**
     * Cleanup winsock.
     */
    ~Winsock();
};
}
