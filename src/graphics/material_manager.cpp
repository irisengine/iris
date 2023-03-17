////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/material_manager.h"

#include <array>
#include <cstddef>
#include <span>

namespace iris
{

std::span<std::byte> MaterialManager::create_property_buffer()
{
    auto &new_buffer = property_buffers_.emplace_back();
    return {std::begin(new_buffer), std::end(new_buffer)};
}

}
