////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#include "core/vector3.h"
#include "graphics/render_graph/node.h"
#include "graphics/sampler.h"
#include "graphics/texture.h"
#include "graphics/texture_usage.h"

namespace iris
{
class ShaderCompiler;

enum class UVMode
{
    VERTEX_DATA,
    SCREEN_SPACE
};

/**
 * Implementation of Node which provides access to a texture. The compiler will
 * sample this texture for the current fragments UV, using this as input to
 * another node will produce a four float value (RGBA).
 */
class TextureNode : public Node
{
  public:
    /**
     * Create a new TextureNode.
     *
     * @param texture
     *   Texture to provide access to.
     */
    TextureNode(const Texture *texture, UVMode = UVMode::VERTEX_DATA);

    /**
     * Create a new TextureNode.
     *
     * @param path
     *   Path of texture.
     *
     * @param usage
     *   The usage of the texture.
     *
     * @param sampler
     *   Sampler to use for this texture.
     */
    TextureNode(
        const std::string &path,
        TextureUsage usage = TextureUsage::IMAGE,
        const Sampler *sampler = nullptr,
        UVMode = UVMode::VERTEX_DATA);

    ~TextureNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get texture.
     *
     * @returns
     *   Texture.
     */
    const Texture *texture() const;

    UVMode uv_mode() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Texture. */
    const Texture *texture_;

    UVMode uv_mode_;
};
}
