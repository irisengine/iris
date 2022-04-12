////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/texture_node.h"

#include <cstddef>

#include "core/root.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/sampler.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace iris
{

TextureNode::TextureNode(const Texture *texture, UVSource uv_source)
    : texture_(texture)
    , uv_source_(uv_source)
{
}

TextureNode::TextureNode(const std::string &path, TextureUsage usage, const Sampler *sampler, UVSource uv_source)
    : texture_(Root::texture_manager().load(path, usage, sampler))
    , uv_source_(uv_source)
{
}

void TextureNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

const Texture *TextureNode::texture() const
{
    return texture_;
}

UVSource TextureNode::uv_source() const
{
    return uv_source_;
}

std::size_t TextureNode::hash() const
{
    return combine_hash(reinterpret_cast<std::ptrdiff_t>(texture_), uv_source_, "texture_node");
}

}
