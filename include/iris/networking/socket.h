#pragma once

#include <cstddef>
#include <optional>

#include "core/data_buffer.h"

namespace iris
{

/**
 * Interface for a socket. This is an object that can read and write bytes
 * from/to another socket object (possibly on a separate machine).
 */
class Socket
{
  public:
    // default
    virtual ~Socket() = default;

    /**
     * Try and read count bytes if they are available (this should be a
     * non-blocking call).
     *
     * Note that if not all requested bytes are available it is down to the
     * implementation whether this is treated as error or just the bytes
     * read are returned.
     *
     * @param count
     *   Amount of bytes to read.
     *
     * @returns
     *   DataBuffer of bytes if read succeeded, otherwise empty optional.
     */
    virtual std::optional<DataBuffer> try_read(std::size_t count) = 0;

    /**
     * Read count bytes (this should be a blocking call).
     *
     * Note that if not all requested bytes are available it is down to the
     * implementation whether this is treated as error or just the bytes
     * read are returned.
     *
     * @param count
     *   Amount of bytes to read.
     *
     * @returns
     *   DataBuffer of bytes read.
     */
    virtual DataBuffer read(std::size_t count) = 0;

    /**
     * Write DataBuffer to socket.
     *
     * @param buffer
     *   Bytes to write.
     */
    virtual void write(const DataBuffer &buffer) = 0;

    /**
     * Write bytes to socket.
     *
     * @param data
     *   Pointer to bytes to write.
     *
     * @param size
     *   Amount of bytes to write.
     */
    virtual void write(const std::byte *data, std::size_t size) = 0;
};

}
