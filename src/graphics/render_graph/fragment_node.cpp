////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/fragment_node.h"

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

FragmentNode::FragmentNode(FragmentDataType fragment_data_type)
    : fragment_data_type_(fragment_data_type)
    , swizzle_(std::nullopt)
{
}

FragmentNode::FragmentNode(FragmentDataType fragment_data_type, std::string_view swizzle)
    : fragment_data_type_(fragment_data_type)
    , swizzle_(swizzle)
{
}

void FragmentNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

FragmentDataType FragmentNode::fragment_data_type() const
{
    return fragment_data_type_;
}

std::optional<std::string> FragmentNode::swizzle() const
{
    return swizzle_;
}

std::size_t FragmentNode::hash() const
{
    return combine_hash(fragment_data_type_, swizzle_, "fragment_node");
}

}
