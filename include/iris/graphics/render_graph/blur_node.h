#pragma once

#include <memory>

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{
class Compiler;

/**
 * Implementation of Node that performs a basic blur on an input texture.
 */
class BlurNode : public Node
{
  public:
    /**
     * Create a new BlurNode
     *
     * @param input_node
     *   Texture to blur.
     */
    BlurNode(TextureNode *input_node);

    ~BlurNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(Compiler &compiler) const override;

    /**
     * Get input texture.
     *
     * @returns
     *   Input texture.
     */
    TextureNode *input_node() const;

  private:
    /** Input texture. */
    TextureNode *input_node_;
};
}
