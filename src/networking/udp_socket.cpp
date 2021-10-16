////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "networking/udp_socket.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "log/log.h"
#include "networking/networking.h"
#include "networking/socket.h"

namespace iris
{

UdpSocket::UdpSocket(const std::string &address, std::uint16_t port)
    : socket_()
    , address_()
    , address_length_(0)
{
    LOG_ENGINE_INFO("udp_socket", "creating socket ({}:{})", address, port);

    // create socket
    socket_ = {::socket(AF_INET, SOCK_DGRAM, 0), CloseSocket};
    if (!socket_)
    {
        throw Exception("socket failed");
    }

    // configure address
    std::memset(&address_, 0x0, sizeof(address_));
    address_length_ = sizeof(address_);
    address_.sin_family = AF_INET;
    address_.sin_port = htons(port);

    // convert address from text to binary
    if (::inet_pton(AF_INET, address.c_str(), &address_.sin_addr.s_addr) != 1)
    {
        throw Exception("failed to convert ip address");
    }

    // enable socket reuse
    int reuse = 1;
    if (::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof(reuse)) < 0)
    {
        throw iris::Exception("setsockopt failed");
    }

    LOG_ENGINE_INFO("udp_socket", "connected!");
}

UdpSocket::UdpSocket(struct sockaddr_in socket_address, socklen_t socket_length, SocketHandle socket)
    : socket_(socket, nullptr)
    , address_(socket_address)
    , address_length_(socket_length)
{
}

std::optional<DataBuffer> UdpSocket::try_read(std::size_t count)
{
    std::optional<DataBuffer> out = DataBuffer(count);

    set_blocking(socket_, false);

    // perform non-blocking read
    auto read = ::recvfrom(
        socket_,
        reinterpret_cast<char *>(out->data()),
        static_cast<int>(out->size()),
        0,
        reinterpret_cast<struct sockaddr *>(&address_),
        &address_length_);

    if (read == -1)
    {
        // read failed but not because there was no data
        if (!last_call_blocked())
        {
            throw iris::Exception("read failed");
        }

        // no data, so reset optional
        out.reset();
    }
    else
    {
        // resize buffer to amount of data read
        out->resize(read);
    }

    return out;
}

DataBuffer UdpSocket::read(std::size_t count)
{
    DataBuffer buffer(count);

    set_blocking(socket_, true);

    // perform blocking read
    auto read = ::recvfrom(
        socket_,
        reinterpret_cast<char *>(buffer.data()),
        static_cast<int>(buffer.size()),
        0,
        reinterpret_cast<struct sockaddr *>(&address_),
        &address_length_);

    if (read == -1)
    {
        throw Exception("recvfrom failed");
    }

    // resize buffer to amount of data read
    buffer.resize(read);

    return buffer;
}

void UdpSocket::write(const DataBuffer &buffer)
{
    write(buffer.data(), buffer.size());
}

void UdpSocket::write(const std::byte *data, std::size_t size)
{
    if (::sendto(
            socket_,
            reinterpret_cast<const char *>(data),
            static_cast<int>(size),
            0,
            reinterpret_cast<struct sockaddr *>(&address_),
            address_length_) != size)
    {
        throw Exception("sendto failed");
    }
}

}
