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
 *  - Packets are sequenced but may have gaps
 *  - No duplicates
 * 
 * e.g.
 *  received packets : 1, 1, 3, 2, 1, 5
 *  yielded packets  : 1, 3, 5
 */
class UnreliableSequencedChannel : public Channel
{
    public:

        /**
         * Construct a new UnreliableSequencedChannel.
         */
        UnreliableSequencedChannel();

        // default
        ~UnreliableSequencedChannel() override = default;

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

    private:

        /** The minimum sequence number to yield for received packets. */
        std::uint16_t min_sequence_;

        /** The sequence number for the next sent packet. */
        std::uint16_t send_sequence_;
};

}
