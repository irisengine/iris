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
 * Enumeration of possible render command types.
 */
enum class RenderCommandType : std::uint8_t
{
    UPLOAD_TEXTURE,
    PASS_START,
    DRAW,
    PASS_END,
    PRESENT
};

}