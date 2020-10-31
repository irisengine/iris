#include <algorithm>
#include <cstdint>
#include <gtest/gtest.h>
#include <iterator>
#include <tuple>
#include <vector>

#include "core/data_buffer.h"
#include "networking/channel/unreliable_unordered_channel.h"
#include "networking/packet.h"

#include "helper.h"

TEST(unreliable_unordered_channel, in_queue_single)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
    });
    iris::UnreliableUnorderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_TRUE(channel.yield_send_queue().empty());
}

TEST(unreliable_unordered_channel, in_queue_multi)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::UnreliableUnorderedChannel channel{};

    for (const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_TRUE(channel.yield_send_queue().empty());
}

TEST(unreliable_unordered_channel, in_queue_multi_early_yield)
{
    const auto in_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::UnreliableUnorderedChannel channel{};
    std::vector<std::vector<iris::Packet>> yielded_packets{};

    channel.enqueue_send(in_packets[0u]);
    channel.enqueue_send(in_packets[1u]);
    yielded_packets.emplace_back(channel.yield_send_queue());
    yielded_packets.emplace_back(channel.yield_send_queue());
    channel.enqueue_send(in_packets[2u]);
    yielded_packets.emplace_back(channel.yield_send_queue());
    yielded_packets.emplace_back(channel.yield_send_queue());

    ASSERT_EQ(yielded_packets.size(), 4u);
    ASSERT_EQ(yielded_packets[0u].size(), 2u);
    ASSERT_EQ(
        yielded_packets[0u],
        std::vector<iris::Packet>(
            std::cbegin(in_packets), std::cbegin(in_packets) + 2u));
    ASSERT_TRUE(yielded_packets[1u].empty());
    ASSERT_EQ(yielded_packets[2u].size(), 1u);
    ASSERT_EQ(
        yielded_packets[2u],
        std::vector<iris::Packet>(
            std::cbegin(in_packets) + 2u, std::cend(in_packets)));
    ASSERT_TRUE(yielded_packets[3u].empty());
}

TEST(unreliable_unordered_channel, out_queue_single)
{
    const auto out_packets = create_packets({
        {0u, iris::PacketType::DATA},
    });
    iris::UnreliableUnorderedChannel channel{};

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), out_packets);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_unordered_channel, out_queue_multi)
{
    const auto out_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::UnreliableUnorderedChannel channel{};

    for (const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), out_packets);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_unordered_channel, out_queue_multi_early_yield)
{
    const auto out_packets = create_packets({
        {0u, iris::PacketType::DATA},
        {1u, iris::PacketType::DATA},
        {2u, iris::PacketType::DATA},
    });
    iris::UnreliableUnorderedChannel channel{};
    std::vector<std::vector<iris::Packet>> yielded_packets{};

    channel.enqueue_receive(out_packets[0u]);
    channel.enqueue_receive(out_packets[1u]);
    yielded_packets.emplace_back(channel.yield_receive_queue());
    yielded_packets.emplace_back(channel.yield_receive_queue());
    channel.enqueue_receive(out_packets[2u]);
    yielded_packets.emplace_back(channel.yield_receive_queue());
    yielded_packets.emplace_back(channel.yield_receive_queue());

    ASSERT_EQ(yielded_packets.size(), 4u);
    ASSERT_EQ(yielded_packets[0u].size(), 2u);
    ASSERT_EQ(
        yielded_packets[0u],
        std::vector<iris::Packet>(
            std::cbegin(out_packets), std::cbegin(out_packets) + 2u));
    ASSERT_TRUE(yielded_packets[1u].empty());
    ASSERT_EQ(yielded_packets[2u].size(), 1u);
    ASSERT_EQ(
        yielded_packets[2u],
        std::vector<iris::Packet>(
            std::cbegin(out_packets) + 2u, std::cend(out_packets)));
    ASSERT_TRUE(yielded_packets[3u].empty());
}
