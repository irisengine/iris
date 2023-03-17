////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/property_node.h"

#include <string>

#include "core/utils.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

PropertyNode::PropertyNode(const std::string &name)
    : name_(name)
{
}

std::string PropertyNode::name() const
{
    return name_;
}

void PropertyNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t PropertyNode::hash() const
{
    return combine_hash(name_, "property_node");
}

}
