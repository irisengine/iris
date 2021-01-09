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
