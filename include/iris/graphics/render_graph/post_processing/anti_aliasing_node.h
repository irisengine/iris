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
class AntiAliasingNode : public RenderNode
{
  public:
    /**
     * Create a new AntiAliasingNode.
     *
     * @param input
     *   Colour input for RenderNide.
     */
    AntiAliasingNode(TextureNode *input);

    ~AntiAliasingNode() override = default;

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
};

}
