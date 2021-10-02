#include "graphics/render_graph/texture_node.h"

#include <cstddef>

#include "core/root.h"
#include "core/vector3.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"

namespace iris
{

TextureNode::TextureNode(Texture *texture)
    : texture_(texture)
{
}

TextureNode::TextureNode(const std::string &path, TextureUsage usage)
    : texture_(Root::texture_manager().load(path, usage))
{
}

void TextureNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Texture *TextureNode::texture() const
{
    return texture_;
}

}
