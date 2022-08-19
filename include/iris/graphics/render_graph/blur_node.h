////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{
class ShaderCompiler;

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
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get input texture.
     *
     * @returns
     *   Input texture.
     */
    TextureNode *input_node() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Input texture. */
    TextureNode *input_node_;
};
}
