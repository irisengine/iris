#include "graphics/render_graph/texture_node.h"

#include <cstddef>

#include "core/vector3.h"
#include "graphics/render_graph/compiler.h"
#include "graphics/texture.h"
#include "graphics/texture_factory.h"

namespace iris
{

TextureNode::TextureNode(Texture *texture)
    : texture_(texture)
{
}

TextureNode::TextureNode(const std::string &path)
    : texture_(iris::texture_factory::load(path))
{
}

void TextureNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

Texture *TextureNode::texture() const
{
    return texture_;
}

}
