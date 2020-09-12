#include "networking/udp_accepting_socket.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "log/log.h"
#include "networking/posix/auto_socket.h"
#include "networking/posix/client_connection.h"
#include "networking/socket.h"

namespace iris
{

struct UdpAcceptingSocket::implementation
{
    std::vector<std::unique_ptr<ClientConnection>> connections;
    AutoSocket socket = -1;
};

UdpAcceptingSocket::UdpAcceptingSocket(const std::string &address, std::uint32_t port)
    : impl_(std::make_unique<implementation>())
{
    LOG_ENGINE_INFO("udp_accepting_socket", "creating server socket ({}:{})", address, port);

    // create socket
    impl_->socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(impl_->socket < 0)
    {
        throw Exception("socket failed");
    }

    // configure address
    struct sockaddr_storage address_storage;
    socklen_t address_length = sizeof(address_storage);
    std::memset(&address_storage, 0x0, address_length);

    auto *addr = reinterpret_cast<struct sockaddr_in*>(&address_storage);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    // convert address from text to binary
    if(::inet_pton(AF_INET, address.c_str(), &addr->sin_addr) != 1)
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
        throw iris::Exception("setsockopt failed");
    }

    // bind socket so we can accept connections
    if(::bind(impl_->socket, (struct sockaddr*)&addr, address_length) == -1)
    {
        throw Exception("bind failed");
    }

    LOG_ENGINE_INFO("udp_accepting_socket", "connected!");
}

UdpAcceptingSocket::~UdpAcceptingSocket() = default;

Socket* UdpAcceptingSocket::accept()
{
    struct sockaddr_storage address;
    socklen_t length = sizeof(address);

    static constexpr auto new_connection_size = 1024u;
    DataBuffer buffer(new_connection_size);

    // block and wait for a new connection
    const auto read = ::recvfrom(
        impl_->socket,
        buffer.data(),
        buffer.size(),
        0,
        reinterpret_cast<struct sockaddr*>(&address),
        &length);
    
    if(read == -1)
    {
        throw Exception("recvfrom failed");
    }

    // resize buffer to amount of data read
    buffer.resize(read);

    // create new connection object, with the data we have already read
    // this is necessary as it is not possible to accept a udp connection without
    // also reading data
    impl_->connections.emplace_back(std::make_unique<ClientConnection>(
        address,
        length,
        buffer,
        static_cast<int>(impl_->socket)));

    LOG_ENGINE_INFO("udp_accepting_socket", "new connection");

    return impl_->connections.back().get();
}

}
