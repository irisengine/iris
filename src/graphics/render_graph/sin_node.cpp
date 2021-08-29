#include "graphics/render_graph/sin_node.h"

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

}
