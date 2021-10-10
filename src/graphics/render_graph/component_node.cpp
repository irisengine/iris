////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/component_node.h"

#include <memory>
#include <string>

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

ComponentNode::ComponentNode(Node *input_node, const std::string &component)
    : input_node_(input_node)
    , component_(component)
{
}

void ComponentNode::accept(ShaderCompiler &compiler) const
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
