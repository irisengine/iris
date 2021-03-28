#include "graphics/render_graph/texture_node.h"

#include <cstddef>

#include "core/vector3.h"
#include "graphics/render_graph/compiler.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace iris
{

TextureNode::TextureNode(Texture *texture)
    : texture_(texture)
{
}

TextureNode::TextureNode(const std::string &path)
    : texture_(TextureManager::load(path))
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
