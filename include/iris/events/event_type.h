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
 * Enumeration of possible Event types.
 */
enum class EventType : std::uint32_t
{
    QUIT,
    KEYBOARD,
    MOUSE,
    MOUSE_BUTTON,
    TOUCH
};

}
