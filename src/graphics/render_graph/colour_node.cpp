#include "graphics/render_graph/colour_node.h"

#include "core/vector3.h"
#include "graphics/render_graph/compiler.h"

namespace iris
{

ColourNode::ColourNode(const Vector3 &colour)
    : colour_(colour)
{
}

void ColourNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

Vector3 ColourNode::colour() const
{
    return colour_;
}

}
