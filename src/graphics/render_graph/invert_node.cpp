////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/invert_node.h"

#include <memory>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

InvertNode::InvertNode(Node *input_node)
    : input_node_(input_node)
{
}

void InvertNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *InvertNode::input_node() const
{
    return input_node_;
}

}
