////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/post_processing/anti_aliasing_node.h"

#include <cstddef>

#include "graphics/render_graph/shader_compiler.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

AntiAliasingNode::AntiAliasingNode(TextureNode *input)
    : RenderNode()
{
    set_colour_input(input);
}

void AntiAliasingNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t AntiAliasingNode::hash() const
{
    return combine_hash(colour_input_, "anti_aliasing_node");
}

}
