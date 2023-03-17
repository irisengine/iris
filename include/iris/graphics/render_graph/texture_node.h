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

/**
 * Enumeration of possible sources of UV data.
 */
enum class UVSource
{
    /** Source from vertex data. */
    VERTEX_DATA,

    /** Source from screen space of fragment being rendered. */
    SCREEN_SPACE,

    /** Source from a node. */
    NODE
};

/**
 * Implementation of Node which provides access to a texture.
 */
class TextureNode : public Node
{
  public:
    /**
     * Create a new TextureNode.
     *
     * @param texture
     *   Texture to provide access to.
     *
     * @param uv_source
     *   Source of UV data.
     *
     * @param uv_input
     *   Optional node to calculate uv, only valid if uv_source is NODE.
     */
    TextureNode(const Texture *texture, UVSource uv_source = UVSource::VERTEX_DATA, const Node *uv_input = nullptr);

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
     * Get source of UV data.
     *
     * @returns
     *   UV data source.
     */
    UVSource uv_source() const;

    /**
     * Get node for uv input.
     *
     * @returns
     *   Node if source is NODE, otherwise nullptr.
     */
    const Node *uv_input() const;

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

    /** Source of UV data. */
    UVSource uv_source_;

    /** Optional node for uv. */
    const Node *uv_input_;
};
}
