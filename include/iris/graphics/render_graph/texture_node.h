#pragma once

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
    TextureNode(Texture *texture);

    /**
     * Create a new TextureNode.
     *
     * @param path
     *   Path of texture.
     */
    TextureNode(
        const std::string &path,
        TextureUsage usage = TextureUsage::IMAGE);

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
    Texture *texture() const;

  private:
    /** Texture. */
    Texture *texture_;
};
}
