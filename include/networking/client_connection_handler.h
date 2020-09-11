#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <map>

#include "core/data_buffer.h"
#include "networking/socket.h"
#include "networking/channel/channel.h"
#include "jobs/concurrent_queue.h"

namespace eng
{

/**
 * This class provides an abstraction over the low-level handling of connecting
 * to a server.
 * It automatically handles:
 *  - making a connection
 *  - handshake
 *  - clock sync
 *  - sending/receiving data
 * 
 * This is all done with the lightweight Packet protocol (see
 * server_connection_handler.h for details on the protocol).
 */
class ClientConnectionHandler
{
    public:

        /**
         * Create a new ClientConnectionHandler.
         * 
         * @param socker
         *   The underlying socket to use.
         */
        ClientConnectionHandler(std::unique_ptr<Socket> socket);

        /**
         * Try and read data from the supplied channel.
         * 
         * @param channel_type
         *   Channel to read from.
         * 
         * @returns
         *   DataBuffer of bytes if read succeeded, otherwise empty optional.
         */   
        std::optional<DataBuffer> try_read(ChannelType channel_type);

        /**
         * Send data to the server on the supplied channel.
         * 
         * @param data
         *   Data to send.
         * 
         * @param channel_type
         *   Channel to send data on
         */
        void send(const DataBuffer &data, ChannelType channel_type);

        /**
         * Unique id of the client (as set by server).
         * 
         * @returns
         *   Server id.
         */
        std::uint32_t id() const;

        /**
         * Estimate of the lag between the client and server. This is the round
         * trip time for a message to get to the server and back. Can change if
         * the server issues a sync.
         * 
         * @returns
         *   Estimate of lag.
         */
        std::chrono::milliseconds lag() const;

    private:
        
        /** Underlying socket. */
        std::unique_ptr<Socket> socket_;

        /** Unique id of this client. */
        std::uint32_t id_;

        /** Estimate of lag between client and server. */
        std::chrono::milliseconds lag_;

        /** Map of channel types to channel objects. */
        std::map<ChannelType, std::unique_ptr<Channel>> channels_;

        /** Map of channel types to message queues. */
        std::map<ChannelType, std::unique_ptr<ConcurrentQueue<DataBuffer>>> queues_;
};

}
