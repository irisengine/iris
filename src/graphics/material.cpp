////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/material.h"

#include <cstddef>
#include <span>

#include "graphics/material_manager.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

Material::Material(const RenderGraph *render_graph)
    : property_buffer_()
{
    if (render_graph != nullptr)
    {
        property_buffer_ = render_graph->property_buffer();
    }
}

std::span<std::byte> Material::property_buffer() const
{
    return property_buffer_;
}

}
