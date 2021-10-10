////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/arithmetic_node.h"

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

ArithmeticNode::ArithmeticNode(Node *value1, Node *value2, ArithmeticOperator arithmetic_operator)
    : value1_(value1)
    , value2_(value2)
    , arithmetic_operator_(arithmetic_operator)
{
}

void ArithmeticNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *ArithmeticNode::value1() const
{
    return value1_;
}

Node *ArithmeticNode::value2() const
{
    return value2_;
}

ArithmeticOperator ArithmeticNode::arithmetic_operator() const
{
    return arithmetic_operator_;
}

}
