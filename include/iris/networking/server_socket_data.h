////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/data_buffer.h"
#include "networking/socket.h"

namespace iris
{

/**
 * Struct for data returned from a ServerSocket read.
 */
struct ServerSocketData
{
    /** A Socket which can be used to communicate with the client. */
    Socket *client;

    /** The data read. */
    DataBuffer data;

    /** Whether this is a new connection or not. */
    bool new_connection;
};

}
