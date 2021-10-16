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
