#include <gtest/gtest.h>

#include "core/exception.h"
#include "networking/packet.h"
#include "networking/packet_type.h"

static const eng::DataBuffer test_data{
    static_cast<std::byte>(0x0),
    static_cast<std::byte>(0x1),
    static_cast<std::byte>(0x2)};

TEST(packet, construct_invalid)
{
    eng::Packet p{ };

    ASSERT_FALSE(p.is_valid());
}

TEST(packet, construct_normal)
{
    eng::Packet p{ eng::PacketType::DATA, eng::ChannelType::RELIABLE_ORDERED, test_data };

    ASSERT_TRUE(p.is_valid());
    ASSERT_EQ(p.type(), eng::PacketType::DATA);
    ASSERT_EQ(p.channel(), eng::ChannelType::RELIABLE_ORDERED);
    ASSERT_EQ(p.body_buffer(), test_data);
    ASSERT_EQ(p.body_size(), test_data.size());
    ASSERT_EQ(p.sequence(), 0u);
    ASSERT_EQ(eng::DataBuffer(p.body(), p.body() + p.body_size()), test_data);
    ASSERT_EQ(std::memcmp(p.data(), &p, p.packet_size()), 0u);
}

TEST(packet, resize_too_large)
{
    eng::Packet p{ eng::PacketType::DATA, eng::ChannelType::RELIABLE_ORDERED, test_data };

    ASSERT_THROW(p.resize(sizeof(p) + 1u), eng::Exception);
}

TEST(packet, sequence)
{
    eng::Packet p{ eng::PacketType::DATA, eng::ChannelType::RELIABLE_ORDERED, test_data };

    p.set_sequence(10u);

    ASSERT_EQ(p.sequence(), 10u);
}

TEST(packet, equality)
{
    eng::Packet p1{ eng::PacketType::DATA, eng::ChannelType::RELIABLE_ORDERED, test_data };
    eng::Packet p2{ eng::PacketType::DATA, eng::ChannelType::RELIABLE_ORDERED, test_data };

    ASSERT_EQ(p1, p2);
}

TEST(packet, inequality)
{
    eng::Packet p1{ eng::PacketType::HELLO, eng::ChannelType::RELIABLE_ORDERED, test_data };
    eng::Packet p2{ eng::PacketType::DATA, eng::ChannelType::RELIABLE_ORDERED, test_data };

    ASSERT_NE(p1, p2);
}
