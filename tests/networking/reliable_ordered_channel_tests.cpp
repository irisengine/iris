////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <vector>

#include "networking/channel/reliable_ordered_channel.h"
#include "networking/packet.h"

#include "helper.h"

TEST(reliable_ordered_channel, unacked_packet_is_resent)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(reliable_ordered_channel, packet_sequence_set)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
    });
    const auto expected = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), expected);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(reliable_ordered_channel, single_ack_single_packet)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
    });
    const auto out_packets = create_packets({
        {0u, iris::PacketType::ACK},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_TRUE(channel.yield_send_queue().empty());
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(reliable_ordered_channel, single_ack_multi_packet)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
    });
    const auto out_packets = create_packets({
        {1u, iris::PacketType::ACK},
    });
    const auto expected = create_packets({
        {0u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), expected);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(reliable_ordered_channel, multi_ack_single_packet)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
    });
    const auto out_packets = create_packets({
        {1u, iris::PacketType::ACK},
        {2u, iris::PacketType::ACK},
        {3u, iris::PacketType::ACK},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(reliable_ordered_channel, multi_ack_multi_packet)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
    });
    const auto out_packets = create_packets({
        {4u, iris::PacketType::ACK},
        {4u, iris::PacketType::ACK},
        {4u, iris::PacketType::ACK},
        {1u, iris::PacketType::ACK},
        {2u, iris::PacketType::ACK},
        {1u, iris::PacketType::ACK},
        {3u, iris::PacketType::ACK},
        {3u, iris::PacketType::ACK},
        {1u, iris::PacketType::ACK},
        {0u, iris::PacketType::ACK},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_TRUE(channel.yield_send_queue().empty());
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(reliable_ordered_channel, single_out_acked)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::ACK},
    });
    const auto out_packets = create_packets({
        {0u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), out_packets);
    ASSERT_EQ(channel.yield_send_queue(), in_packets);
}

TEST(reliable_ordered_channel, multi_out_acked)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::ACK},
        {1u, iris::PacketType::ACK},
        {2u, iris::PacketType::ACK},
    });
    const auto out_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), out_packets);
    ASSERT_EQ(channel.yield_send_queue(), in_packets);
}

TEST(reliable_ordered_channel, multi_out_acked_unordered)
{
    const auto in_packets = create_packets({
        {2u, iris::PacketType::ACK},
        {0u, iris::PacketType::ACK},
        {1u, iris::PacketType::ACK},
        {3u, iris::PacketType::ACK},
    });
    const auto out_packets = create_packets({
        {2u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {3u, iris::PacketType::DATA},
    });
    const auto expected = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
        {3u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), expected);
    ASSERT_EQ(channel.yield_send_queue(), in_packets);
}

TEST(reliable_ordered_channel, multi_out_acked_early_yield)
{
    const auto in_packets = create_packets({
        {3u, iris::PacketType::ACK},
        {1u, iris::PacketType::ACK},
        {0u, iris::PacketType::ACK},
        {2u, iris::PacketType::ACK},
    });
    const auto out_packets = create_packets({
        {3u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {0u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    const auto expected1 = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
    });
    const auto expected2 = create_packets({
        {2u, iris::PacketType::DATA},
        {3u, iris::PacketType::DATA},
    });
    iris::ReliableOrderedChannel channel{};

    channel.enqueue_receive(out_packets[0u]);
    channel.enqueue_receive(out_packets[1u]);
    channel.enqueue_receive(out_packets[2u]);
    const auto out_queue1 = channel.yield_receive_queue();
    channel.enqueue_receive(out_packets[3u]);
    const auto out_queue2 = channel.yield_receive_queue();
    const auto out_queue3 = channel.yield_receive_queue();

    ASSERT_EQ(out_queue1, expected1);
    ASSERT_EQ(out_queue2, expected2);
    ASSERT_TRUE(out_queue3.empty());
    ASSERT_EQ(channel.yield_send_queue(), in_packets);
}
