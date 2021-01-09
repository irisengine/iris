#include "networking/udp_socket.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>

#include "core/auto_release.h"
#include "core/data_buffer.h"
#include "core/exception.h"
#include "log/log.h"
#include "networking/networking.h"
#include "networking/socket.h"

namespace iris
{

struct UdpSocket::implementation
{
    AutoRelease<SocketHandle, INVALID_SOCKET> socket;
    struct sockaddr_in address;
    socklen_t address_length = 0;
};

UdpSocket::UdpSocket(const std::string &address, std::uint16_t port)
    : impl_(std::make_unique<implementation>())
{
    LOG_ENGINE_INFO("udp_socket", "creating socket ({}:{})", address, port);

    // create socket
    impl_->socket = {::socket(AF_INET, SOCK_DGRAM, 0), CloseSocket};
    if (!impl_->socket)
    {
        throw Exception("socket failed");
    }

    // configure address
    std::memset(&impl_->address, 0x0, sizeof(impl_->address));
    impl_->address_length = sizeof(impl_->address);
    impl_->address.sin_family = AF_INET;
    impl_->address.sin_port = htons(port);

    // convert address from text to binary
    if (::inet_pton(
            AF_INET, address.c_str(), &impl_->address.sin_addr.s_addr) != 1)
    {
        throw Exception("failed to convert ip address");
    }

    // enable socket reuse
    int reuse = 1;
    if (::setsockopt(
            impl_->socket,
            SOL_SOCKET,
            SO_REUSEADDR,
            reinterpret_cast<const char *>(&reuse),
            sizeof(reuse)) < 0)
    {
        throw iris::Exception("setsockopt failed");
    }

    LOG_ENGINE_INFO("udp_socket", "connected!");
}

UdpSocket::UdpSocket(
    struct sockaddr_in socket_address,
    socklen_t socket_length,
    SocketHandle socket)
    : impl_(std::make_unique<implementation>())
{
    impl_->socket = {socket, nullptr};
    impl_->address = socket_address;
    impl_->address_length = socket_length;
}

UdpSocket::~UdpSocket() = default;
UdpSocket::UdpSocket(UdpSocket &&) = default;
UdpSocket &UdpSocket::operator=(UdpSocket &&) = default;

std::optional<DataBuffer> UdpSocket::try_read(std::size_t count)
{
    std::optional<DataBuffer> out = DataBuffer(count);

    set_blocking(impl_->socket, false);

    // perform non-blocking read
    auto read = ::recvfrom(
        impl_->socket,
        reinterpret_cast<char *>(out->data()),
        static_cast<int>(out->size()),
        0,
        reinterpret_cast<struct sockaddr *>(&impl_->address),
        &impl_->address_length);

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

    set_blocking(impl_->socket, true);

    // perform blocking read
    auto read = ::recvfrom(
        impl_->socket,
        reinterpret_cast<char *>(buffer.data()),
        static_cast<int>(buffer.size()),
        0,
        reinterpret_cast<struct sockaddr *>(&impl_->address),
        &impl_->address_length);

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
            impl_->socket,
            reinterpret_cast<const char *>(data),
            static_cast<int>(size),
            0,
            reinterpret_cast<struct sockaddr *>(&impl_->address),
            impl_->address_length) != size)
    {
        throw Exception("sendto failed");
    }
}

}
