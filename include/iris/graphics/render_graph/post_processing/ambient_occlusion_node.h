////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>

#include "graphics/post_processing_description.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

class ShaderCompiler;

/**
 * Implementation of Node which applies Screen Space Ambient Occlusion (SSAO) to the final output of a render pass.
 *
 * Note that internally this well be converted to a pre-processing step which calculates the occlusion, this will then
 * be integrated into the ambient lighting pass. For simplicity it is presented to the user as a post processing step.
 *
 */
class AmbientOcclusionNode : public RenderNode
{
  public:
    /**
     * Create a new AmbientOcclusionNode.
     *
     * @param input
     *   Colour input for RenderNode.
     *
     * @param normal_texture
     *   Texture containing screen space normals.
     *
     * @param position_texture
     *   Texture containing screen space positions.
     *
     * @param description
     *   User settings for effect.
     */
    AmbientOcclusionNode(
        TextureNode *input,
        TextureNode *normal_texture,
        TextureNode *position_texture,
        AmbientOcclusionDescription description);

    ~AmbientOcclusionNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get screen space normals texture.
     *
     * @returns
     *   Texture containing screen space normals.
     */
    TextureNode *normal_texture() const;

    /**
     * Get screen space positions texture.
     *
     * @returns
     *   Texture containing screen space positions.
     */
    TextureNode *position_texture() const;

    /**
     * Get description of effect.
     *
     * @returns
     *   User settings for effect.
     */
    AmbientOcclusionDescription description() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Texture containing screen space normals. */
    TextureNode *normal_texture_;

    /** Texture containing screen space positions. */
    TextureNode *position_texture_;

    /** User settings for effect. */
    AmbientOcclusionDescription description_;
};

}
