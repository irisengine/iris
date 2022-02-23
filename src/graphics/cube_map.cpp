////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/cube_map.h"

#include <cstdint>

namespace iris
{

CubeMap::CubeMap(std::uint32_t index)
    : index_(index)
{
}

std::uint32_t CubeMap::index() const
{
    return index_;
}

}
