#include "graphics/render_graph/combine_node.h"

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

}
