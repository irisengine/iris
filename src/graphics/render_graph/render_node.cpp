#include "graphics/render_graph/render_node.h"

#include <memory>

#include "graphics/render_graph/compiler.h"

namespace iris
{

RenderNode::RenderNode()
    : colour_input_(nullptr)
    , normal_input_(nullptr)
    , position_input_(nullptr)
    , shadow_map_inputs_()
    , depth_only_(false)
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

Node *RenderNode::shadow_map_input(std::size_t index) const
{
    return index < shadow_map_inputs_.size() ? shadow_map_inputs_.at(index)
                                             : nullptr;
}

void RenderNode::add_shadow_map_input(Node *input)
{
    shadow_map_inputs_.emplace_back(input);
}

bool RenderNode::is_depth_only() const
{
    return depth_only_;
}

void RenderNode::set_depth_only(bool depth_only)
{
    depth_only_ = depth_only;
}

}
