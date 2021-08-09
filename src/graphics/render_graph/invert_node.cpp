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
