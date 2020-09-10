#pragma once

#include <tuple>
#include <cstdint>
#include <vector>

#include "core/data_buffer.h"
#include "networking/packet.h"

static const eng::DataBuffer test_data{
    static_cast<std::byte>(0xaa),
    static_cast<std::byte>(0xbb),
    static_cast<std::byte>(0xcc),
};

/**
 * Helper method to create a collection of packets with supplied sequence
 * numbers and types.
 * 
 * @param packet_sequence
 *   Collection of sequence numbers and types.
 * 
 * @returns
 *   Packets created using supplied data.
 */
inline std::vector<eng::Packet> create_packets(
    const std::vector<std::tuple<std::uint16_t, eng::PacketType>> &packet_sequence)
{
    std::vector<eng::Packet> packets;

    for(const auto &[sequence, type] : packet_sequence)
    {
        // acks don't have data
        const auto data = type == eng::PacketType::ACK
            ? eng::DataBuffer{ }
            : test_data;

        eng::Packet packet{ type, eng::ChannelType::RELIABLE_ORDERED, data };
        packet.set_sequence(sequence);
        packets.emplace_back(packet);
    }

    return packets;
}
