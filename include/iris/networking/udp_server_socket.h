#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "core/auto_release.h"
#include "networking/networking.h"
#include "networking/server_socket.h"
#include "networking/server_socket_data.h"

namespace iris
{

/**
 * Implementation of ServerSocket which accepts UDP connections.
 *
 * The ServerSocketData provides a socket for communicating back with the
 * client. This behaves as any normal socket and can be read from and
 * written to.
 *
 * Note that in general one should avoid reading from the returned client
 * Socket object. This is because internally the client socket and this
 * ServerSocket hold the same underlying network primitive. Which means reading
 * from one could potentially interfere with the other. Best practice is to
 * only every read from the UdpServerSocket and write back with the returned
 * client Socket.
 */
class UdpServerSocket : public ServerSocket
{
  public:
    /**
     * Construct a new UdpServerSocket.
     *
     * @param address
     *   Address to listen to for connections.
     *
     * @param port
     *   Port to listen on.
     */
    UdpServerSocket(const std::string &address, std::uint32_t port);

    UdpServerSocket(const UdpServerSocket &) = delete;
    UdpServerSocket &operator=(const UdpServerSocket &) = delete;

    ~UdpServerSocket() override = default;

    /**
     * Block and wait for data.
     *
     * @returns
     *    A ServerSocketData for the read client and data.
     */
    ServerSocketData read() override;

  private:
    /** Map of address to Socket for clients. */
    std::map<std::uint32_t, std::unique_ptr<Socket>> connections_;

    /** Underlying server socket. */
    AutoRelease<SocketHandle, INVALID_SOCKET> socket_;
};

}
