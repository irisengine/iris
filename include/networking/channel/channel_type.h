#pragma once

#include <cstdint>

namespace iris
{

/**
 * Enumeration of channel types.
 */
enum class ChannelType : std::uint8_t
{
    INVAlID,
    UNRELIABLE_UNORDERED,
    UNRELIABLE_SEQUENCED,
    RELIABLE_ORDERED,
};

}
