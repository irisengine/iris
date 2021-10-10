////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/vertex_position_node.h"

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

VertexPositionNode::VertexPositionNode()
{
}

void VertexPositionNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

}
