#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>

#include "core/real.h"
#include "networking/socket.h"

namespace iris
{

/**
 * An implementation of Socket that uses IPC to send/receive data. This class
 * is designed to send packets reliably and with minimal latency but allows the
 * user to simulate various properties of an unreliable connection.
 */
class SimulatedSocket : public Socket
{
    public:

        /**
         * Construct a new SimulatedSocket.
         * 
         * @param client_queue_name
         *   The name of the IPC queue for client messages. Must be numerical.
         * 
         * @param server_queue_name
         *   The name of the IPC queue for server messages. Must be numerical.
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
         */
        SimulatedSocket(
            const std::string &client_queue_name,
            const std::string &server_queue_name,
            std::chrono::milliseconds delay,
            std::chrono::milliseconds jitter,
            real drop_rate);

        // defined in implementation
        ~SimulatedSocket() override;

        // deleted
        SimulatedSocket(const SimulatedSocket&) = delete;
        SimulatedSocket& operator=(const SimulatedSocket&) = delete;

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

        /** Packet delay. */
        std::chrono::milliseconds delay_;

        /** Delay jitter. */
        std::chrono::milliseconds jitter_;

        /** Packet drop rate. */
        real drop_rate_;
};

}

