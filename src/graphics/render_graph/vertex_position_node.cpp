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
