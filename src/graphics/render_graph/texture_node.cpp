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

TextureNode::TextureNode(const Texture *texture, UVMode uv_mode)
    : texture_(texture)
    , uv_mode_(uv_mode)
{
}

TextureNode::TextureNode(const std::string &path, TextureUsage usage, const Sampler *sampler, UVMode uv_mode)
    : texture_(Root::texture_manager().load(path, usage, sampler))
    , uv_mode_(uv_mode)
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

UVMode TextureNode::uv_mode() const
{
    return uv_mode_;
}

std::size_t TextureNode::hash() const
{
    return combine_hash(reinterpret_cast<std::ptrdiff_t>(texture_), uv_mode_, "texture_node");
}

}
