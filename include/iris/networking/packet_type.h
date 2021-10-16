////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace iris
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
