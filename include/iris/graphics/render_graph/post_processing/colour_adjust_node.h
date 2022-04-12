////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#include "graphics/post_processing_description.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

class ShaderCompiler;

/**
 * Implementation of Node which performs various colour adjustments to the final output of a render pass.
 *
 * See ColourAdjustDescription for possible adjustment options.
 */
class ColourAdjustNode : public RenderNode
{
  public:
    /**
     * Create a new ColourAdjustNode.
     *
     * @param input
     *   Colour input for RenderNode.
     *
     * @param description
     *   User settings for effect.
     */
    ColourAdjustNode(TextureNode *input, ColourAdjustDescription description);

    ~ColourAdjustNode() override = default;

    /**
     * Get description of effect.
     *
     * @returns
     *   User settings for effect.
     */
    ColourAdjustDescription description() const;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Gamma amount. */
    float gamma_;

    /** User settings for effect. */
    ColourAdjustDescription description_;
};

}
