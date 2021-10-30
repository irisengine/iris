////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "networking/udp_server_socket.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "core/auto_release.h"
#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "log/log.h"
#include "networking/networking.h"
#include "networking/server_socket_data.h"
#include "networking/socket.h"
#include "networking/udp_socket.h"

namespace iris
{

UdpServerSocket::UdpServerSocket(const std::string &address, std::uint32_t port)
    : connections_()
    , socket_()
{
    LOG_ENGINE_INFO("udp_server_socket", "creating server socket ({}:{})", address, port);

    // create socket
    socket_ = {::socket(AF_INET, SOCK_DGRAM, 0), CloseSocket};
    ensure(socket_ == INVALID_SOCKET, "socket failed");

    // configure address
    struct sockaddr_in address_storage = {0};
    socklen_t address_length = sizeof(struct sockaddr_in);
    memset(&address_storage, 0x0, address_length);

    address_storage.sin_family = AF_INET;
    inet_pton(AF_INET, address.c_str(), &address_storage.sin_addr.s_addr);
    address_storage.sin_port = htons(static_cast<std::uint16_t>(port));

    // enable multicast
    int reuse = 1;
    ensure(
        ::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof(reuse)) == 0,
        "setsockopt failed");

    // bind socket so we can accept connections
    ensure(::bind(socket_, reinterpret_cast<struct sockaddr *>(&address_storage), address_length) == 0, "bind failed");

    LOG_ENGINE_INFO("udp_server_socket", "connected!");
}

ServerSocketData UdpServerSocket::read()
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    static constexpr auto new_connection_size = 1024u;
    DataBuffer buffer(new_connection_size);

    // block and wait for a new connection
    const auto read = ::recvfrom(
        socket_,
        reinterpret_cast<char *>(buffer.data()),
        static_cast<int>(buffer.size()),
        0,
        reinterpret_cast<struct sockaddr *>(&address),
        &length);

    ensure(read != -1, "recvfrom failed");

    // resize buffer to amount of data read
    buffer.resize(read);

    const auto byte_address = address.sin_addr.s_addr;

    auto new_connection = false;

    if (connections_.count(byte_address) == 0u)
    {
        connections_[byte_address] = std::make_unique<UdpSocket>(address, length, socket_.get());

        new_connection = true;

        LOG_ENGINE_INFO("udp_server_socket", "new connection");
    }

    return {connections_[byte_address].get(), buffer, new_connection};
}

}
