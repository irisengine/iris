#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "core/data_buffer.h"
#include "networking/networking.h"
#include "networking/socket.h"

namespace iris
{

/**
 * Implementation of Socket using UDP. This class simply sends UDP datagrams and
 * makes no guarantees about deliveries or order.
 */
class UdpSocket : public Socket
{
  public:
    /**
     * Construct a new UdpSocket to the supplied address and port.
     *
     * @param address
     *   Address to communicate with.
     *
     * @param port
     *   Port on address to communicate with.
     */
    UdpSocket(const std::string &address, std::uint16_t port);

    /**
     * Construct a new UdpSocket from an existing BSD socket. This is non-owning
     * and will not close the SocketHandle when it goes out of scope.
     *
     * This constructor annoyingly breaks the abstraction around the BSD socket
     * primitives but is a necessary evil.
     *
     * @param socket_address
     *   BSD socket struct.
     *
     * @param socket_length
     *   Length (in bytes) of socket_address
     *
     * @param socket
     *   SocketHandle to take a non-owning copy of.
     */
    UdpSocket(
        struct sockaddr_in socket_address,
        socklen_t socket_length,
        SocketHandle socket);

    // disabled
    UdpSocket(const UdpSocket &) = delete;
    UdpSocket &operator=(const UdpSocket &) = delete;

    // defined in implementation
    ~UdpSocket() override;
    UdpSocket(UdpSocket &&);
    UdpSocket &operator=(UdpSocket &&);

    /**
     * Try and read requested number bytes. Will return all bytes read up to
     * count, but maybe less.
     *
     * @param count
     *   Maximum number of bytes to read.
     *
     * @returns
     *   DataBuffer of bytes if read succeeded, otherwise empty optional.
     */
    std::optional<DataBuffer> try_read(std::size_t count) override;

    /**
     * Block and read up to count bytes. May return less.
     *
     * @param count
     *   Maximum number of bytes to read.
     *
     * @returns
     *   DataBuffer of bytes read.
     */
    DataBuffer read(std::size_t count) override;

    /**
     * Write DataBuffer to socket.
     *
     * @param buffer
     *   Bytes to write.
     */
    void write(const DataBuffer &buffer) override;

    /**
     * Write bytes to socket.
     *
     * @param data
     *   Pointer to bytes to write.
     *
     * @param size
     *   Amount of bytes to write.
     */
    void write(const std::byte *data, std::size_t size) override;

  private:
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
