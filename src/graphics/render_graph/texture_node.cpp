////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/texture_node.h"

#include <cstddef>

#include "core/error_handling.h"
#include "core/root.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/sampler.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace iris
{

TextureNode::TextureNode(const Texture *texture, UVSource uv_source, const Node *uv_input)
    : texture_(texture)
    , uv_source_(uv_source)
    , uv_input_(uv_input)
{
    ensure(
        (uv_source_ == UVSource::NODE && uv_input_ != nullptr) ||
            (uv_source_ != UVSource::NODE && uv_input_ == nullptr),
        "invalid configuration");
}

TextureNode::TextureNode(
    const std::string &path,
    TextureUsage usage,
    const Sampler *sampler,
    UVSource uv_source,
    const Node *uv_input)
    : texture_(Root::texture_manager().load(path, usage, sampler))
    , uv_source_(uv_source)
    , uv_input_(uv_input)
{
    ensure(
        (uv_source_ == UVSource::NODE && uv_input_ != nullptr) ||
            (uv_source_ != UVSource::NODE && uv_input_ == nullptr),
        "invalid configuration");
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

const Node *TextureNode::uv_input() const
{
    return uv_input_;
}

std::size_t TextureNode::hash() const
{
    return combine_hash(reinterpret_cast<std::ptrdiff_t>(texture_), uv_source_, uv_input_, "texture_node");
}

}
