////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/vertex_node.h"

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include "core/utils.h"
#include "graphics/render_graph/node.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

VertexNode::VertexNode(VertexDataType vertex_data_type)
    : vertex_data_type_(vertex_data_type)
    , swizzle_(std::nullopt)
{
}

VertexNode::VertexNode(VertexDataType vertex_data_type, std::string_view swizzle)
    : vertex_data_type_(vertex_data_type)
    , swizzle_(swizzle)
{
}

void VertexNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

VertexDataType VertexNode::vertex_data_type() const
{
    return vertex_data_type_;
}

std::optional<std::string> VertexNode::swizzle() const
{
    return swizzle_;
}

std::size_t VertexNode::hash() const
{
    return combine_hash(vertex_data_type_, swizzle_, "vertex_node");
}

}
