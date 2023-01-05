////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/lerp_node.h"

#include <cstddef>

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

LerpNode::LerpNode(Node *input_value1, Node *input_value2, Node *lerp_amount)
    : input_value1_(input_value1)
    , input_value2_(input_value2)
    , lerp_amount_(lerp_amount)
{
}

void LerpNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *LerpNode::input_value1() const
{
    return input_value1_;
}

Node *LerpNode::input_value2() const
{
    return input_value2_;
}

Node *LerpNode::lerp_amount() const
{
    return lerp_amount_;
}

std::size_t LerpNode::hash() const
{
    return combine_hash(input_value1_, input_value2_, lerp_amount_, "lerp_node");
}

}
