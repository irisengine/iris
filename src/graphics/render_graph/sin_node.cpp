////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/sin_node.h"

#include <cstddef>
#include <memory>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

SinNode::SinNode(Node *input_node)
    : input_node_(input_node)
{
}

void SinNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *SinNode::input_node() const
{
    return input_node_;
}

std::size_t SinNode::hash() const
{
    return combine_hash(input_node_, "sin_node");
}

}
