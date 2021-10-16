////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "networking/simulated_server_socket.h"

#include <chrono>
#include <memory>

#include "networking/server_socket_data.h"
#include "networking/simulated_socket.h"

namespace iris
{

SimulatedServerSocket::SimulatedServerSocket(
    std::chrono::milliseconds delay,
    std::chrono::milliseconds jitter,
    float drop_rate,
    ServerSocket *socket)
    : socket_(socket)
    , client_(nullptr)
    , delay_(delay)
    , jitter_(jitter)
    , drop_rate_(drop_rate)
{
}

ServerSocketData SimulatedServerSocket::read()
{
    auto [client_socket, data, new_client] = socket_->read();

    if (!client_)
    {
        client_ = std::make_unique<SimulatedSocket>(delay_, jitter_, drop_rate_, client_socket);
    }

    return {client_.get(), data, new_client};
}

}
