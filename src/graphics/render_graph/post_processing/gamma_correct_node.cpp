////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/post_processing/gamma_correct_node.h"

#include <cstddef>

#include "graphics/render_graph/shader_compiler.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

GammaCorrectNode::GammaCorrectNode(TextureNode *input, float gamma)
    : RenderNode()
    , gamma_(gamma)
{
    set_colour_input(input);
}

float GammaCorrectNode::gamma() const
{
    return gamma_;
}

void GammaCorrectNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t GammaCorrectNode::hash() const
{
    return combine_hash(colour_input_, "gamma_correct_node");
}

}
