#include "graphics/render_graph/render_node.h"

#include <memory>

#include "graphics/render_graph/compiler.h"

namespace iris
{

RenderNode::RenderNode()
    : colour_input_(nullptr)
    , normal_input_(nullptr)
    , position_input_(nullptr)
{
}

void RenderNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

Node *RenderNode::colour_input() const
{
    return colour_input_;
}

void RenderNode::set_colour_input(Node *input)
{
    colour_input_ = input;
}

Node *RenderNode::normal_input() const
{
    return normal_input_;
}

void RenderNode::set_normal_input(Node *input)
{
    normal_input_ = input;
}

Node *RenderNode::position_input() const
{
    return position_input_;
}

void RenderNode::set_position_input(Node *input)
{
    position_input_ = input;
}

}
