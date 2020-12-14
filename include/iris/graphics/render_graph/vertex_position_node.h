#pragma once

#include "graphics/render_graph/node.h"

namespace iris
{
class Compiler;

/**
 * Implementation of Node which provides access to a meshes vertex position.
 */
class VertexPositionNode : public Node
{
  public:
    /**
     * Create a new VertexPositionNode.
     */
    VertexPositionNode();
    ~VertexPositionNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(Compiler &compiler) const override;
};
}
