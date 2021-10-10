////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/blur_node.h"

#include "graphics/render_graph/shader_compiler.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

BlurNode::BlurNode(TextureNode *input_node)
    : input_node_(input_node)
{
}

void BlurNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

TextureNode *BlurNode::input_node() const
{
    return input_node_;
}

}
