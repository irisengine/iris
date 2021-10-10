////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "networking/server_socket_data.h"

namespace iris
{

/**
 * Interface for a server socket. This is a socket that will read data from all
 * clients, returning read data as well as a Socket object to communicate back
 * with them.
 *
 * See documentation in deriving classes for implementation specific caveats.
 */
class ServerSocket
{
  public:
    virtual ~ServerSocket() = default;

    /**
     * Block and wait for data.
     *
     * @returns
     *    A ServerSocketData for the read client and data.
     */
    virtual ServerSocketData read() = 0;
};

}
