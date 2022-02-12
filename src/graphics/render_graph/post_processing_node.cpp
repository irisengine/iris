////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/post_processing_node.h"

#include <cstddef>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

PostProcessingNode::PostProcessingNode(Node *input)
    : RenderNode()
{
    set_colour_input(input);
}

void PostProcessingNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t PostProcessingNode::hash() const
{
    return combine_hash(colour_input_, "post_processing_node");
}

}
