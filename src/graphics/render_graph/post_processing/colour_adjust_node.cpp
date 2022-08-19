////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/post_processing/colour_adjust_node.h"

#include <cstddef>

#include "graphics/post_processing_description.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

ColourAdjustNode::ColourAdjustNode(TextureNode *input, ColourAdjustDescription description)
    : RenderNode()
    , description_(description)
{
    set_colour_input(input);
}

ColourAdjustDescription ColourAdjustNode::description() const
{
    return description_;
}

void ColourAdjustNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t ColourAdjustNode::hash() const
{
    return combine_hash(colour_input_, description_.gamma, description_.tone_map_curve, "colour_adjust_node");
}

}
