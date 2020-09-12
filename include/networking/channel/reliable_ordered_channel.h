#pragma once

#include <cstdint>
#include <deque>
#include <vector>

#include "networking/socket.h"
#include "networking/channel/channel.h"

namespace eng
{

/**
 * Implementation of Channel with the following guarantees:
 *  - Packets are in order
 *  - Packets are guaranteed to arrive
 *  - No gaps
 *  - No duplicates
 * 
 * This is the strictest channel and effectively provides reliable delivery over
 * an unreliable transport.
 * 
 * e.g.
 *  received packets : 1, 1, 3, 2, 1, 5, 4
 *  yielded packets  : 1, 2, 3, 4, 5
 */
class ReliableOrderedChannel : public Channel
{
    public:

        /**
         * Construct a new ReliableOrderedChannel.
         */
        ReliableOrderedChannel();

        // default
        ~ReliableOrderedChannel() override = default;

        /**
         * Enqueue a packet to be sent.
         * 
         * @param packet
         *   Packet to be sent.
         */
        void enqueue_send(Packet packet) override;

        /**
         * Enqueue a received packet.
         * 
         * @param packet
         *   Packet received.
         */
        void enqueue_receive(Packet packet) override;

        /**
         * Yield all packets to be sent, according to the channel guarantees.
         * 
         * @returns
         *   Packets to be send.
         */
        std::vector<Packet> yield_send_queue() override;

        /**
         * Yield all packets that have been received, according to the channel
         * guarantees.
         * 
         * @returns
         *   Packets received.
         */
        std::vector<Packet> yield_receive_queue() override;
        

    private:

        /** The expected sequence number of the next packet. */
        std::uint16_t next_receive_seq_;

        /** The sequence number for the next sent packet. */
        std::uint16_t out_sequence_;
};

}

