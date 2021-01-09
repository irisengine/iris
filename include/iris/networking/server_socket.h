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
