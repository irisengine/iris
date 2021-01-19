#pragma once

#include "core/colour.h"
#include "graphics/render_graph/node.h"

namespace iris
{
class Compiler;

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
    void accept(Compiler &compiler) const override;

    /**
     * Get colour.
     *
     * @returns
     *   Colour.
     */
    Colour colour() const;

  private:
    /** Colour. */
    Colour colour_;
};
}
