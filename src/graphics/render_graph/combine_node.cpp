////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/combine_node.h"

#include <cstddef>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

CombineNode::CombineNode(Node *value1, Node *value2, Node *value3, Node *value4)
    : value1_(value1)
    , value2_(value2)
    , value3_(value3)
    , value4_(value4)
{
}

void CombineNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *CombineNode::value1() const
{
    return value1_;
}

Node *CombineNode::value2() const
{
    return value2_;
}

Node *CombineNode::value3() const
{
    return value3_;
}

Node *CombineNode::value4() const
{
    return value4_;
}

std::size_t CombineNode::hash() const
{
    return combine_hash(value1_, value2_, value3_, value4_, "combine_node");
}

}
