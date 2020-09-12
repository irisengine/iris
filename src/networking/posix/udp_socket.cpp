#include "networking/udp_socket.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "log/log.h"
#include "networking/posix/auto_socket.h"
#include "networking/socket.h"

namespace eng
{

struct UdpSocket::implementation
{
    AutoSocket socket = -1;
    struct sockaddr_storage address;
    socklen_t address_length = 0;
};

UdpSocket::UdpSocket(const std::string &address, std::uint16_t port)
    : impl_(std::make_unique<implementation>())
{
    LOG_ENGINE_INFO("udp_socket", "creating socket ({}:{})", address, port);

    // create socket
    impl_->socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(impl_->socket < 0)
    {
        throw Exception("socket failed");
    }

    // configure address
    std::memset(&impl_->address, 0x0, sizeof(impl_->address));
    auto *addr = reinterpret_cast<struct sockaddr_in*>(&impl_->address);
    impl_->address_length = sizeof(impl_->address);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    // convert address from text to binary
    if(::inet_pton(AF_INET, address.c_str(), addr) != 1)
    {
        throw Exception("failed to convert ip address");
    }

    // enable multicast
    int reuse = 1;
    if(::setsockopt(
        impl_->socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        reinterpret_cast<const char*>(&reuse),
        sizeof(reuse)) < 0)
    {
        throw eng::Exception("setsockopt failed");
    }

    LOG_ENGINE_INFO("udp_socket", "connected!");
}

UdpSocket::~UdpSocket() = default;
UdpSocket::UdpSocket(UdpSocket&&) = default;
UdpSocket& UdpSocket::operator=(UdpSocket&&) = default;

std::optional<DataBuffer> UdpSocket::try_read(std::size_t count)
{
    std::optional<DataBuffer> out = DataBuffer(count);

    // perform non-blocking read
    auto read = ::recvfrom(
        impl_->socket,
        out->data(),
        out->size(),
        MSG_DONTWAIT,
        reinterpret_cast<struct sockaddr*>(&impl_->address),
        &impl_->address_length);

    if(read == -1)
    {
        // read failed but not because there was no data
        if(errno != EAGAIN || errno != EWOULDBLOCK)
        {
            throw eng::Exception("read failed");
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

    // perform blocking read
    auto read = ::recvfrom(
        impl_->socket,
        buffer.data(),
        buffer.size(),
        0,
        reinterpret_cast<struct sockaddr*>(&impl_->address),
        &impl_->address_length);

    if(read == -1)
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
    if(::sendto(
        impl_->socket,
        data,
        size,
        0,
        reinterpret_cast<struct sockaddr*>(&impl_->address),
        impl_->address_length) != size)
    {
        throw Exception("sendto failed");
    }
}

}
