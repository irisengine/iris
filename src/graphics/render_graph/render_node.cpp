////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/render_node.h"

#include <memory>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

RenderNode::RenderNode()
    : colour_input_(nullptr)
    , specular_power_input_(nullptr)
    , specular_amount_input_(nullptr)
    , normal_input_(nullptr)
    , position_input_(nullptr)
    , shadow_map_input_(nullptr)
    , depth_only_(false)
{
}

void RenderNode::accept(ShaderCompiler &compiler) const
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

Node *RenderNode::specular_power_input() const
{
    return specular_power_input_;
}

void RenderNode::set_specular_power_input(Node *input)
{
    specular_power_input_ = input;
}

Node *RenderNode::specular_amount_input() const
{
    return specular_amount_input_;
}

void RenderNode::set_specular_amount_input(Node *input)
{
    specular_amount_input_ = input;
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

Node *RenderNode::shadow_map_input() const
{
    return shadow_map_input_;
}

void RenderNode::set_shadow_map_input(Node *input)
{
    shadow_map_input_ = input;
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
