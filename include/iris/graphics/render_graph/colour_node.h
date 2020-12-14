#pragma once

#include "core/vector3.h"
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
    ColourNode(const Vector3 &colour);

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
    Vector3 colour() const;

  private:
    /** Colour. */
    Vector3 colour_;
};
}
