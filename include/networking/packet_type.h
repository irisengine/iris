#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of packet types.
 */
enum class PacketType : std::uint8_t
{
    INVAlID,
    HELLO,
    CONNECTED,
    DATA,
    ACK,
    SYNC_START,
    SYNC_RESPONSE,
    SYNC_FINISH
};

}
