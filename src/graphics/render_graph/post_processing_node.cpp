#include "graphics/render_graph/post_processing_node.h"

#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

PostProcessingNode::PostProcessingNode(Node *input)
    : RenderNode()
{
    set_colour_input(input);
}

void PostProcessingNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

}
