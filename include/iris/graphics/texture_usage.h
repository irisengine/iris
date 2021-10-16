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
 * Encapsulation of the semantic usage of textures.
 */
enum class TextureUsage : std::uint8_t
{
    IMAGE,
    DATA,
    RENDER_TARGET,
    DEPTH
};

}
