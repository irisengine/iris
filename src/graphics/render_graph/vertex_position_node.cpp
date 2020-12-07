#include "graphics/render_graph/vertex_position_node.h"

#include "graphics/render_graph/compiler.h"

namespace iris
{

VertexPositionNode::VertexPositionNode()
{
}

void VertexPositionNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

}
