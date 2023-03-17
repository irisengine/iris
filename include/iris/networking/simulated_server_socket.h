////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>

#include "core/context.h"
#include "networking/server_socket.h"
#include "networking/server_socket_data.h"
#include "networking/simulated_socket.h"

namespace iris
{

/**
 * An adaptor for ServerSocket which can simulate different network conditions.
 * Note that these conditions are compounded with any properties of the
 * underlying Socket.
 *
 * Supports one clinet.
 */
class SimulatedServerSocket : public ServerSocket
{
  public:
    /**
     * Construct a new SimulatedServerSocket.
     *
     * @param context
     *   Engine context object.
     *
     * @param delay
     *   The fixed delay for all packets.
     *
     * @param jitter
     *   The random variance in delay. All packets will be delayed by:
     *      delay + rand[-jitter, jitter]
     *
     * @param drop_rate
     *   The rate at which packets will be dropped, must be in the range
     *   [0.0, 1.0] -> [no packets dropped, all packets dropped]
     *
     * @param socket
     *   ServerSocket to adapt. Will be used for underlying communication, but
     * with simulated conditions.
     */
    SimulatedServerSocket(
        Context &context,
        std::chrono::milliseconds delay,
        std::chrono::milliseconds jitter,
        float drop_rate,
        ServerSocket *socket);

    ~SimulatedServerSocket() override = default;

    /**
     * Block and wait for data.
     *
     * @returns
     *    A ServerSocketData for the read client and data.
     */
    ServerSocketData read() override;

  private:
    /** Underlying socket. */
    ServerSocket *socket_;

    /** The single client. */
    std::unique_ptr<SimulatedSocket> client_;

    /** Simulated delay. */
    std::chrono::milliseconds delay_;

    /** Simulated jitter. */
    std::chrono::milliseconds jitter_;

    /** Simulated packet drop rate. */
    float drop_rate_;

    /** Engine context. */
    Context &context_;
};

}
