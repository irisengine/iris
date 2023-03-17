////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/binary_operator_node.h"

#include <cstddef>
#include <functional>

#include "core/utils.h"
#include "graphics/render_graph/node.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

BinaryOperatorNode::BinaryOperatorNode(Node *value1, Node *value2, BinaryOperator binary_operator)
    : value1_(value1)
    , value2_(value2)
    , binary_operator_(binary_operator)
{
}

void BinaryOperatorNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *BinaryOperatorNode::value1() const
{
    return value1_;
}

Node *BinaryOperatorNode::value2() const
{
    return value2_;
}

BinaryOperator BinaryOperatorNode::binary_operator() const
{
    return binary_operator_;
}

std::size_t BinaryOperatorNode::hash() const
{
    return combine_hash(value1_, value2_, binary_operator_, "binary_operator_node");
}

}
