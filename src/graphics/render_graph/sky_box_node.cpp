////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/sky_box_node.h"

#include <cstddef>

#include "graphics/cube_map.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

SkyBoxNode::SkyBoxNode(const CubeMap *sky_box)
    : RenderNode()
    , sky_box_(sky_box)
{
}

void SkyBoxNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

const CubeMap *SkyBoxNode::sky_box() const
{
    return sky_box_;
}

std::size_t SkyBoxNode::hash() const
{
    return combine_hash(reinterpret_cast<std::ptrdiff_t>(sky_box_), "sky_box_node");
}

}
