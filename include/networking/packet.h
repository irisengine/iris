#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>

#include "core/data_buffer.h"
#include "networking/channel/channel_type.h"
#include "networking/packet_type.h"

namespace iris
{

/**
 * Class encapsulating a packet the engine will send/receive. This is a
 * low-level primitive used to facilitate the basic protocol the engine provides.
 * It is expected that a user will not create and send/receive these packets
 * directly, but rather use other constructs in the engine to send their own
 * game-specific protocol.
 * 
 * The Packet consists of a header then a fixed body buffer, which can contain
 * arbitrary data (may be smaller than the buffer size).
 * 
 *                               +----------+ -.
 *                               |   type   |  |
 *                               +----------+  |
 *                               | channel  |  |- header
 *                               +----------+  |
 *                               | sequence |  |
 *                            .- +----------+ -'
 *                            |  |          |
 *                            |  |          |
 *                            |  |          |
 *                            |  |   body   |
 *                            |  |          |
 *               body buffer -|  |          |
 *                            |  |          |
 *                            |  |~~~~~~~~~~|- body size
 *                            |  |          |
 *                            |  |          |
 *                            |  |          |
 *                            '- +----------+
 */
class Packet
{
    public:

        /**
         * Construct an invalid Packet. All methods on an invalid packet should
         * be considered undefined except:
         *  - is_valid
         *  - data
         */
        Packet();

        /**
         * Construct a new Packet.
         * 
         * @param type
         *   The type of packet.
         * 
         * @param channel
         *   The channel the packet should be sent on.
         * 
         * @param body
         *   The data of the packet, may be empty.
         */
        Packet(PacketType type, ChannelType channel, const DataBuffer &body);

        /**
         * Get a pointer to the start of the packet.
         * 
         * @returns
         *   Pointer to start of this Packet.
         */
        const std::byte* data() const;

        /**
         * Get a pointer to the start of the packet.
         * 
         * @returns
         *   Pointer to start of this Packet.
         */
        std::byte* data();
        
        /**
         * Get a pointer to the body of the packet.
         * 
         * @returns
         *   Pointer to start of the Packet body.
         */
        const std::byte* body() const;
        
        /**
         * Get a pointer to the body of the packet.
         * 
         * @returns
         *   Pointer to start of the Packet body.
         */
        std::byte* body();
        
        /**
         * Get the contents of the body.
         * 
         * @returns
         *   Body contents.
         */
        DataBuffer body_buffer() const;
        
        /**
         * Get the size of the packet i.e. sizeof(header) + sizeof(body).
         * 
         * Note this may be less than sizeof(Packet) if the body buffer is not
         * full.
         * 
         * @returns
         *   Size of Packet that is filled.
         */
        std::size_t packet_size() const;
        
        /**
         * Get the size of the body i.e. how much of the body buffer is filled.
         * 
         * @returns
         *   Size of body.
         */
        std::size_t body_size() const;
        
        /**
         * Get packet type.
         * 
         * @returns
         *   Type of packet.
         */
        PacketType type() const;
        
        /**
         * Get channel type.
         * 
         * @returns
         *   Type of channel.
         */
        ChannelType channel() const;

        /**
         * Check if Packet is valid.
         * 
         * @returns
         *   True if packet is valid, otherwise false.
         */
        bool is_valid() const;
        
        /**
         * Resize the packet so the body is size bytes.
         * 
         * @param size
         *   New size of body.
         */
        void resize(std::size_t size);
        
        /**
         * Get the sequence number of the Packet.
         * 
         * @returns
         *   Sequence number.
         */
        std::uint16_t sequence() const;
        
        /**
         * Set the sequence number.
         * 
         * @param sequence
         *   New sequence number.
         */
        void set_sequence(std::uint16_t sequence);
        
        /**
         * Equality operator.
         * 
         * @param other
         *   Packet to check for equality.
         * 
         * @returns
         *   True if both Packet objects are the same, otherwise false.
         */
        bool operator==(const Packet &other) const;
        
        /**
         * Inequality operator.
         * 
         * @param other
         *   Packet to check for inequality.
         * 
         * @returns
         *   True if both Packet objects are not the same, otherwise false.
         */
        bool operator!=(const Packet &other) const;
        
        /**
         * Write a Packet to a stream, useful for debugging.
         * 
         * @param out
         *   Stream to write to.
         * 
         * @param packet
         *   Packet to write to stream.
         * 
         * @returns
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(std::ostream &out, const Packet &packet);

    private:

        /**
         * Internal struct for Packet header.
         */
        struct Header
        {
            /**
             * Construct a new Header.
             * 
             * @param type
             *   Packet type.
             * 
             * @param channel
             *   Channel type.
             */
            Header(PacketType type, ChannelType channel)
                : type(type),
                  channel(channel),
                  sequence(0u)
            {
                // check the header has no padding
                static_assert(sizeof(Header) == sizeof(type) +
                                                sizeof(channel) +
                                                sizeof(sequence), "header has padding");
            }

            /** Type of packet. */
            PacketType type;

            /** Type of channel. */
            ChannelType channel;

            /** Sequence number. */
            std::uint16_t sequence;
        };

        /** Packet header. */
        Header header_;

        /** Packet body buffer. */
        std::byte body_[128 - sizeof(Header)];

        // anything after here should be considered local bookkeeping and will
        // not be transmitted when a Packet is sent.

        /** Size of body buffer used. */
        std::size_t size_;
};

}
