#pragma once

#include <cstddef>
#include <optional>

#include <netinet/in.h>
#include <sys/socket.h>

#include "core/data_buffer.h"
#include "networking/socket.h"

namespace eng
{

/**
 * Implementation of Socket for connections accepted with UdpSocketServer.
 * Functions almost the same as UdpSocket except that it sis constructed with
 * data already read from the underlying socket and will return that on the
 * first read call.
 * 
 * This object should not be instantiated directly, it will be created by
 * UdpAcceptingSocket.
 */
class ClientConnection : public Socket
{
    public:

        /**
         * Construct a new ClientConnection.
         * 
         * @param address
         *   Address object of connection.
         * 
         * @param length
         *   Length in bytes of address object.
         * 
         * @param existing_data
         *   Data already ready from underlying socket.
         * 
         * @param socket
         *   Socket file descriptor.
         */
        ClientConnection(
            const struct sockaddr_storage &address,
            socklen_t length,
            const DataBuffer &existing_data,
            int socket);

        // default
        ~ClientConnection() override = default;

        // disabled
        ClientConnection(const ClientConnection&) = delete;
        ClientConnection& operator=(const ClientConnection&) = delete;

        /**
         * Try and read requested number bytes. Will return all bytes read up to
         * count, but maybe less.
         * 
         * First read call will return any data that was read when connection
         * was accepted.
         * 
         * @param count
         *   Maximum number of bytes to read.
         * 
         * @returns
         *   DataBuffer of bytes if read succeeded, otherwise empty optional.
         */
        std::optional<eng::DataBuffer> try_read(std::size_t) override;

        /**
         * Block and read up to count bytes. May return less.
         * 
         * First read call will return any data that was read when connection
         * was accepted.
         * 
         * @param count
         *   Maximum number of bytes to read.
         * 
         * @returns
         *   DataBuffer of bytes read.
         */
        eng::DataBuffer read(std::size_t) override;

        /**
         * Write DataBuffer to socket.
         * 
         * @param buffer
         *   Bytes to write.
         */
        void write(const eng::DataBuffer &buffer) override;

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

        /** Data that was read when connection was accepted. */
        DataBuffer existing_data_;

        /** Address object of connection. */
        struct sockaddr_storage address_;

        /** Length in bytes of address object. */
        socklen_t length_;

        /** Socket file descriptor. */
        int socket_;
};

}
