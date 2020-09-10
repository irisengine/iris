#include <gtest/gtest.h>

#include <vector>

#include "networking/packet.h"
#include "networking/channel/unreliable_sequenced_channel.h"

#include "helper.h"

TEST(unreliable_sequenced_channel, in_queue_single)
{
    const auto in_packets = create_packets({
        { 0u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_TRUE(channel.yield_send_queue().empty());
}

TEST(unreliable_sequenced_channel, in_queue_multi)
{
    const auto in_packets = create_packets({
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), in_packets);
    ASSERT_TRUE(channel.yield_send_queue().empty());
}

TEST(unreliable_sequenced_channel, in_queue_multi_incrementing_seq)
{
    const auto in_packets = create_packets({
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
    });
    const auto expected = create_packets({
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : in_packets)
    {
        channel.enqueue_send(packet);
    }

    ASSERT_EQ(channel.yield_send_queue(), expected);
    ASSERT_TRUE(channel.yield_send_queue().empty());
}


TEST(unreliable_sequenced_channel, in_queue_multi_early_yield)
{
    const auto in_packets = create_packets({
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };
    std::vector<std::vector<eng::Packet>> yielded_packets{ };

    channel.enqueue_send(in_packets[0u]);
    channel.enqueue_send(in_packets[1u]);
    yielded_packets.emplace_back(channel.yield_send_queue());
    yielded_packets.emplace_back(channel.yield_send_queue());
    channel.enqueue_send(in_packets[2u]);
    yielded_packets.emplace_back(channel.yield_send_queue());
    yielded_packets.emplace_back(channel.yield_send_queue());

    ASSERT_EQ(yielded_packets.size(), 4u);
    ASSERT_EQ(yielded_packets[0u].size(), 2u);
    ASSERT_EQ(yielded_packets[0u], std::vector<eng::Packet>(std::cbegin(in_packets), std::cbegin(in_packets) + 2u));
    ASSERT_TRUE(yielded_packets[1u].empty());
    ASSERT_EQ(yielded_packets[2u].size(), 1u);
    ASSERT_EQ(yielded_packets[2u], std::vector<eng::Packet>(std::cbegin(in_packets) + 2u, std::cend(in_packets)));
    ASSERT_TRUE(yielded_packets[3u].empty());
}

TEST(unreliable_sequenced_channel, out_queue_single)
{
    const auto out_packets = create_packets({
        { 0u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), out_packets);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_sequenced_channel, out_queue_multi)
{
    const auto out_packets = create_packets({
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), out_packets);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_sequenced_channel, out_queue_duplicates)
{
    const auto out_packets = create_packets({
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
    });
    const auto expected = create_packets({
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 2u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), expected);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_sequenced_channel, out_queue_unordered)
{
    const auto out_packets = create_packets({
        { 1u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 3u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 7u, eng::PacketType::DATA },
    });
    const auto expected = create_packets({
        { 1u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 7u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), expected);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_sequenced_channel, out_queue_unordered_and_duplicates)
{
    const auto out_packets = create_packets({
        { 1u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 3u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 7u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
    });
    const auto expected = create_packets({
        { 1u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 7u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };

    for(const auto &packet : out_packets)
    {
        channel.enqueue_receive(packet);
    }

    ASSERT_EQ(channel.yield_receive_queue(), expected);
    ASSERT_TRUE(channel.yield_receive_queue().empty());
}

TEST(unreliable_sequenced_channel, out_queue_unordered_and_duplicates_early_yield)
{
    const auto out_packets = create_packets({
        { 1u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 1u, eng::PacketType::DATA },
        { 3u, eng::PacketType::DATA },
        { 0u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
        { 7u, eng::PacketType::DATA },
        { 5u, eng::PacketType::DATA },
    });
    const auto expected1 = create_packets({
        { 1u, eng::PacketType::DATA },
        { 4u, eng::PacketType::DATA },
    });
    const auto expected2 = create_packets({
        { 5u, eng::PacketType::DATA },
        { 7u, eng::PacketType::DATA },
    });
    eng::UnreliableSequencedChannel channel{ };
    std::vector<std::vector<eng::Packet>> yielded_packets{ };

    channel.enqueue_receive(out_packets[0u]);
    channel.enqueue_receive(out_packets[1u]);
    channel.enqueue_receive(out_packets[2u]);
    channel.enqueue_receive(out_packets[3u]);
    channel.enqueue_receive(out_packets[4u]);
    yielded_packets.emplace_back(channel.yield_receive_queue());
    yielded_packets.emplace_back(channel.yield_receive_queue());
    channel.enqueue_receive(out_packets[5u]);
    channel.enqueue_receive(out_packets[6u]);
    channel.enqueue_receive(out_packets[7u]);
    channel.enqueue_receive(out_packets[8u]);
    channel.enqueue_receive(out_packets[9u]);
    channel.enqueue_receive(out_packets[10u]);
    channel.enqueue_receive(out_packets[11u]);
    channel.enqueue_receive(out_packets[12u]);
    channel.enqueue_receive(out_packets[12u]);
    channel.enqueue_receive(out_packets[13u]);
    yielded_packets.emplace_back(channel.yield_receive_queue());
    yielded_packets.emplace_back(channel.yield_receive_queue());
    channel.enqueue_receive(out_packets[14u]);
    yielded_packets.emplace_back(channel.yield_receive_queue());

    ASSERT_EQ(yielded_packets.size(), 5u);
    ASSERT_EQ(yielded_packets[0u], expected1);
    ASSERT_TRUE(yielded_packets[1u].empty());
    ASSERT_EQ(yielded_packets[2u], expected2);
    ASSERT_TRUE(yielded_packets[3u].empty());
    ASSERT_TRUE(yielded_packets[4u].empty());
}

