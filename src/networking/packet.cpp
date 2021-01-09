#include "networking/packet.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "networking/channel/channel_type.h"
#include "networking/packet_type.h"

namespace iris
{

Packet::Packet()
    : Packet(PacketType::INVAlID, ChannelType::INVAlID, {})
{
}

Packet::Packet(PacketType type, ChannelType channel, const DataBuffer &body)
    : header_(type, channel)
    , body_()
    , size_(body.size())
{
    if (body.size() > sizeof(body_))
    {
        throw Exception("body too large");
    }

    std::memcpy(body_, body.data(), body.size());
}

Packet::Packet(const DataBuffer &raw_packet)
    : Packet()
{
    const auto size_to_copy = std::min<std::size_t>(128ul, raw_packet.size());

    std::memcpy(this, raw_packet.data(), size_to_copy);

    size_ = size_to_copy - sizeof(Header);
}

const std::byte *Packet::data() const
{
    return reinterpret_cast<const std::byte *>(this);
}

std::byte *Packet::data()
{
    return reinterpret_cast<std::byte *>(this);
}

const std::byte *Packet::body() const
{
    return body_;
}

std::byte *Packet::body()
{
    return body_;
}

DataBuffer Packet::body_buffer() const
{
    return DataBuffer(body(), body() + size_);
}

std::size_t Packet::packet_size() const
{
    // sanity check the Packet class is the expected size
    static_assert(
        sizeof(Packet) == 128 + sizeof(size_), "packet has invalid size");

    return sizeof(Header) + body_size();
}

std::size_t Packet::body_size() const
{
    return size_;
}

PacketType Packet::type() const
{
    return header_.type;
}

ChannelType Packet::channel() const
{
    return header_.channel;
}

bool Packet::is_valid() const
{
    return header_.type != PacketType::INVAlID;
}

std::uint16_t Packet::sequence() const
{
    return header_.sequence;
}

void Packet::set_sequence(std::uint16_t sequence)
{
    header_.sequence = sequence;
}

bool Packet::operator==(const Packet &other) const
{
    return (
        (packet_size() == other.packet_size()) &&
        (std::memcmp(data(), other.data(), packet_size()) == 0));
}

bool Packet::operator!=(const Packet &other) const
{
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &out, const Packet &packet)
{
    switch (packet.header_.type)
    {
        case PacketType::INVAlID: out << "INVALID"; break;
        case PacketType::HELLO: out << "HELLO"; break;
        case PacketType::CONNECTED: out << "CONNECTED"; break;
        case PacketType::DATA: out << "DATA"; break;
        case PacketType::ACK: out << "ACK"; break;
        default: out << "UNKNOWN"; break;
    }

    out << ", ";

    switch (packet.header_.channel)
    {
        case ChannelType::INVAlID: out << "INVALID"; break;
        case ChannelType::UNRELIABLE_UNORDERED:
            out << "UNRELIABLE_UNORDERED";
            break;
        case ChannelType::UNRELIABLE_SEQUENCED:
            out << "UNRELIABLE_SEQUENCED";
            break;
        case ChannelType::RELIABLE_ORDERED: out << "RELIABLE_ORDERED"; break;
        default: out << "UNKNOWN"; break;
    }

    out << ", ";

    out << "[" << packet.header_.sequence << "]";
    out << "  ";
    out << packet.size_;
    out << " | ";

    out << std::hex;

    for (auto i = 0u;
         i < std::min(static_cast<std::uint32_t>(packet.size_), 8u);
         ++i)
    {
        out << static_cast<int>(packet.body_[i]) << " ";
    }

    out << std::dec << std::endl;

    return out;
}
}