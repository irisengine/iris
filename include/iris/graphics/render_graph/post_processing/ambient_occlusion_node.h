////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

class ShaderCompiler;

/**
 */
class AmbientOcclusionNode : public RenderNode
{
  public:
    /**
     * Create a new AmbientOcclusionNode.
     *
     * @param input
     *   Colour input for RenderNide.
     */
    AmbientOcclusionNode(TextureNode *input, TextureNode *normal_texture, TextureNode *position_texture);

    ~AmbientOcclusionNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    TextureNode *normal_texture() const;

    TextureNode *position_texture() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    TextureNode *normal_texture_;
    TextureNode *position_texture_;
};

}
