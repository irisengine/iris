////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#include "core/vector3.h"
#include "graphics/render_graph/node.h"
#include "graphics/texture.h"
#include "graphics/texture_usage.h"

namespace iris
{
class ShaderCompiler;

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
    TextureNode(const Texture *texture);

    /**
     * Create a new TextureNode.
     *
     * @param path
     *   Path of texture.
     */
    TextureNode(const std::string &path, TextureUsage usage = TextureUsage::IMAGE);

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
};
}
