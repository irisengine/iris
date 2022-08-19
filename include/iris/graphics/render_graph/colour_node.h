////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#include "core/colour.h"
#include "graphics/render_graph/node.h"

namespace iris
{
class ShaderCompiler;

/**
 * Implementation of Node which represents a colour.
 */
class ColourNode : public Node
{
  public:
    /**
     * Create a new ColourNode.
     *
     * @param colour
     *   The colour to represent.
     */
    ColourNode(const Colour &colour);

    ~ColourNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get colour.
     *
     * @returns
     *   Colour.
     */
    Colour colour() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Colour. */
    Colour colour_;
};
}
