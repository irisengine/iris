////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/time_node.h"

#include <functional>
#include <string_view>

#include "graphics/render_graph/shader_compiler.h"

using namespace std::literals;

namespace iris
{

void TimeNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

std::size_t TimeNode::hash() const
{
    return std::hash<std::string_view>{}("time_node"sv);
}

}
