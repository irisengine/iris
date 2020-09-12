#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "networking/accepting_socket.h"

namespace eng
{

// forward declaration
class Socket;

/**
 * Implementation of AcceptingSocket which accepts UDP connections.
 */
class UdpAcceptingSocket : public AcceptingSocket
{
    public:
        /**
         * Construct a new UdpAcceptingSocket.
         * 
         * @param address
         *   Address to listen to for connections.
         * 
         * @param port
         *   Port to listen on.
         */
        UdpAcceptingSocket(const std::string &address, std::uint32_t port);

        // disabled
        UdpAcceptingSocket(const UdpAcceptingSocket&) = delete;
        UdpAcceptingSocket& operator=(const UdpAcceptingSocket&) = delete;

        // defined in implementation
        ~UdpAcceptingSocket() override;
        UdpAcceptingSocket(UdpAcceptingSocket&&);
        UdpAcceptingSocket& operator=(UdpAcceptingSocket&&);

        /**
         * Block and wait for a new connection.
         * 
         * @returns
         *   A Socket object for communicating with the new connection.
         */
        Socket* accept() override;

    private:

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

