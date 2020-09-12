#include "networking/posix/client_connection.h"

#include <cerrno>
#include <cstddef>
#include <optional>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "core/exception.h"

namespace iris
{

ClientConnection::ClientConnection(
    const struct sockaddr_storage &address,
    socklen_t length,
    const DataBuffer &existing_data,
    int socket)
    : existing_data_(existing_data),
      address_(address),
      length_(length),
      socket_(socket)
{ }

std::optional<DataBuffer> ClientConnection::try_read(std::size_t count)
{
    std::optional<DataBuffer> out;

    if(!existing_data_.empty())
    {
        // we already have data from construction, so return that then clear
        out = existing_data_;
        existing_data_.clear();
    }
    else
    {
        out = DataBuffer(count);

        // perform non-blocking read
        auto read = ::recvfrom(
            socket_,
            out->data(),
            out->size(),
            MSG_DONTWAIT,
            reinterpret_cast<struct sockaddr*>(&address_),
            &length_);

        if(read == -1)
        {
            // read failed but not because there was no data
            if(errno != EAGAIN || errno != EWOULDBLOCK)
            {
                throw Exception("read failed");
            }

            // no data, so reset optional
            out.reset();
        }
        else
        {
            // resize buffer to amount of data read
            out->resize(read);
        }
    }

    return out;
}

DataBuffer ClientConnection::read(std::size_t)
{
    DataBuffer out(1024);

    if(!existing_data_.empty())
    {
        // we already have data from construction, so return that then clear
        out = existing_data_;
        existing_data_.clear();
    }
    else
    {
        // perform blocking read
        auto read = ::recvfrom(
            socket_,
            out.data(),
            out.size(),
            0,
            reinterpret_cast<struct sockaddr*>(&address_),
            &length_);

        if(read == -1)
        {
            throw Exception("read failed");
        }

        // perform blocking read
        out.resize(read);
    }

    return out;
}

void ClientConnection::write(const DataBuffer &buffer)
{
    write(buffer.data(), buffer.size());
}

void ClientConnection::write(const std::byte *data, std::size_t size)
{
    if(::sendto(
        socket_,
        data,
        size,
        0,
        reinterpret_cast<struct sockaddr*>(&address_),
        length_) == -1)
    {
        throw Exception("sendto failed");
    }
}

}
