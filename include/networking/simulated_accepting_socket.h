#pragma once

#include <algorithm>
#include <chrono>

#include "core/real.h"
#include "networking/accepting_socket.h"
#include "networking/simulated_socket.h"

namespace iris
{

/**
 * An implementation of AcceptingSocket that uses IPC to create connections.
 * This class only supports a single connected client.
 */
class SimulatedAcceptingSocket : public AcceptingSocket
{
    public:

        /**
         * Construct a new SimulatedAcceptingSocket.
         * 
         * @param client_queue_name
         *   The name of the IPC queue for client messages. Must be numerical.
         * 
         * @param server_queue_name
         *   The name of the IPC queue for server messages. Must be numerical.
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
         */
        SimulatedAcceptingSocket(
            const std::string &client_queue_name,
            const std::string &server_queue_name,
            std::chrono::milliseconds delay,
            std::chrono::milliseconds jitter,
            real drop_rate);

        // default
        ~SimulatedAcceptingSocket() override = default;

        /**
         * Returns a Socket for a single client.
         * 
         * Note that due to the way this is implemented that subsequent calls
         * to accept() will not block, but rather return a nullptr.
         * 
         * @returns
         *   A Socket object for communicating with the new connection.
         */
        Socket* accept() override;

    private:

        /** The single client. */
        std::unique_ptr<SimulatedSocket> client_;
};

}
