#include "graphics/render_graph/composite_node.h"

#include <cstddef>
#include <memory>

#include "core/vector3.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/texture.h"

namespace iris
{

CompositeNode::CompositeNode(
    Node *colour1,
    Node *colour2,
    Node *depth1,
    Node *depth2)
    : colour1_(colour1)
    , colour2_(colour2)
    , depth1_(depth1)
    , depth2_(depth2)
{
}

void CompositeNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Node *CompositeNode::colour1() const
{
    return colour1_;
}

Node *CompositeNode::colour2() const
{
    return colour2_;
}

Node *CompositeNode::depth1() const
{
    return depth1_;
}

Node *CompositeNode::depth2() const
{
    return depth2_;
}

}
