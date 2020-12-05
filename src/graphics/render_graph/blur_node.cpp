#include "graphics/render_graph/blur_node.h"

#include "graphics/render_graph/compiler.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

BlurNode::BlurNode(TextureNode *input_node)
    : input_node_(input_node)
{
}

void BlurNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

TextureNode *BlurNode::input_node() const
{
    return input_node_;
}

}
