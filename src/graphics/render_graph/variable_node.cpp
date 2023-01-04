////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/variable_node.h"

#include <cstddef>
#include <memory>
#include <string>

#include "core/utils.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

VariableNode::VariableNode(const std::string &name)
    : name_(name)
{
}

std::string VariableNode::name() const
{
    return name_;
}

void VariableNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t VariableNode::hash() const
{
    return combine_hash(name_, "variable_node");
}

}
