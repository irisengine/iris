////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include "core/data_buffer.h"
#include "networking/packet.h"

static const iris::DataBuffer test_data{
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
inline std::vector<iris::Packet>
create_packets(const std::vector<std::tuple<std::uint16_t, iris::PacketType>>
                   &packet_sequence)
{
    std::vector<iris::Packet> packets;

    for (const auto &[sequence, type] : packet_sequence)
    {
        // acks don't have data
        const auto data =
            type == iris::PacketType::ACK ? iris::DataBuffer{} : test_data;

        iris::Packet packet{type, iris::ChannelType::RELIABLE_ORDERED, data};
        packet.set_sequence(sequence);
        packets.emplace_back(packet);
    }

    return packets;
}
