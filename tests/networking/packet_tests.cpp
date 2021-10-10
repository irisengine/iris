////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "core/exception.h"
#include "networking/packet.h"
#include "networking/packet_type.h"

static const iris::DataBuffer test_data{
    static_cast<std::byte>(0x0),
    static_cast<std::byte>(0x1),
    static_cast<std::byte>(0x2)};

TEST(packet, construct_invalid)
{
    iris::Packet p{};

    ASSERT_FALSE(p.is_valid());
}

TEST(packet, construct_normal)
{
    iris::Packet p{
        iris::PacketType::DATA, iris::ChannelType::RELIABLE_ORDERED, test_data};

    ASSERT_TRUE(p.is_valid());
    ASSERT_EQ(p.type(), iris::PacketType::DATA);
    ASSERT_EQ(p.channel(), iris::ChannelType::RELIABLE_ORDERED);
    ASSERT_EQ(p.body_buffer(), test_data);
    ASSERT_EQ(p.body_size(), test_data.size());
    ASSERT_EQ(p.sequence(), 0u);
    ASSERT_EQ(iris::DataBuffer(p.body(), p.body() + p.body_size()), test_data);
    ASSERT_EQ(std::memcmp(p.data(), &p, p.packet_size()), 0u);
}

TEST(packet, construct_raw_packet)
{
    iris::Packet p1{
        iris::PacketType::DATA, iris::ChannelType::RELIABLE_ORDERED, test_data};

    iris::DataBuffer raw_packet{p1.data(), p1.data() + p1.packet_size()};

    iris::Packet p2{raw_packet};

    ASSERT_EQ(p1, p2);
}

TEST(packet, sequence)
{
    iris::Packet p{
        iris::PacketType::DATA, iris::ChannelType::RELIABLE_ORDERED, test_data};

    p.set_sequence(10u);

    ASSERT_EQ(p.sequence(), 10u);
}

TEST(packet, equality)
{
    iris::Packet p1{
        iris::PacketType::DATA, iris::ChannelType::RELIABLE_ORDERED, test_data};
    iris::Packet p2{
        iris::PacketType::DATA, iris::ChannelType::RELIABLE_ORDERED, test_data};

    ASSERT_EQ(p1, p2);
}

TEST(packet, inequality)
{
    iris::Packet p1{
        iris::PacketType::HELLO,
        iris::ChannelType::RELIABLE_ORDERED,
        test_data};
    iris::Packet p2{
        iris::PacketType::DATA, iris::ChannelType::RELIABLE_ORDERED, test_data};

    ASSERT_NE(p1, p2);
}
