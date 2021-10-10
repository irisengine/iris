////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <cstddef>
#include <optional>

#include "jobs/concurrent_queue.h"
#include "networking/socket.h"

namespace iris
{

/**
 * An adaptor for Socket which can simulate different network conditions. Note
 * that these conditions are compounded with any properties of the underlying
 * Socket.
 */
class SimulatedSocket : public Socket
{
  public:
    /**
     * Construct a new SimulatedSocket.
     *
     * @param delay
     *   The fixed delay for all packets.
     *
     * @param jitter
     *   The random variance in delay. All packets will be delayed by:
     *      delay + rand[-jitter, jitter]
     *
     * @param drop_rate
     *   The rate at which packets will be dropped, must be in the range
     *   [0.0, 1.0] -> [no packets dropped, all packets dropped]
     *
     * @param socket
     *   Socket to adapt. Will be used for underlying communication, but with
     *   simulated conditions.
     */
    SimulatedSocket(std::chrono::milliseconds delay, std::chrono::milliseconds jitter, float drop_rate, Socket *socket);

    // defined in implementation
    ~SimulatedSocket() override;

    // deleted
    SimulatedSocket(const SimulatedSocket &) = delete;
    SimulatedSocket &operator=(const SimulatedSocket &) = delete;

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
    /** Packet delay. */
    std::chrono::milliseconds delay_;

    /** Delay jitter. */
    std::chrono::milliseconds jitter_;

    /** Packet drop rate. */
    float drop_rate_;

    /** Underlying socket. */
    Socket *socket_;

    /** Queue of data to send and when. */
    ConcurrentQueue<std::tuple<DataBuffer, std::chrono::steady_clock::time_point>> write_queue_;
};

}
