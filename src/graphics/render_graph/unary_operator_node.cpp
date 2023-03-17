////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/unary_operator_node.h"

#include <cstddef>
#include <memory>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

UnaryOperatorNode::UnaryOperatorNode(Node *input_node, UnaryOperator unary_operator)
    : input_node_(input_node)
    , unary_operator_(unary_operator)
{
}

void UnaryOperatorNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *UnaryOperatorNode::input_node() const
{
    return input_node_;
}

UnaryOperator UnaryOperatorNode::unary_operator() const
{
    return unary_operator_;
}

std::size_t UnaryOperatorNode::hash() const
{
    return combine_hash(input_node_, unary_operator_, "unary_operator_node");
}

}
