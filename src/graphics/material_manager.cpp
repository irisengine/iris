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

std::span<std::byte> MaterialManager::create_property_buffer(const RenderGraph *render_graph)
{
    auto &buffer = property_buffers_[render_graph];

    return {std::begin(buffer), std::end(buffer)};
}

}
