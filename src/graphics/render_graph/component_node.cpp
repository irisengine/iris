#include "graphics/render_graph/component_node.h"

#include <memory>
#include <string>

#include "graphics/render_graph/compiler.h"

namespace iris
{

ComponentNode::ComponentNode(Node *input_node, const std::string &component)
    : input_node_(input_node)
    , component_(component)
{
}

void ComponentNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

Node *ComponentNode::input_node() const
{
    return input_node_;
}

std::string ComponentNode::component() const
{
    return component_;
}

}
