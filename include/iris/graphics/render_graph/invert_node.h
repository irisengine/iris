#pragma once

#include <memory>

#include "graphics/render_graph/node.h"

namespace iris
{
class Compiler;

/**
 * Implementation of Node which inverts the input nodes colour.
 */
class InvertNode : public Node
{
  public:
    /**
     * Create a new InvertNode.
     *
     * @param input_node
     *   Node to invert.
     */
    InvertNode(Node *input_node);

    ~InvertNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(Compiler &compiler) const override;

    /**
     * Get input node.
     *
     * @returns
     *   Input node.
     */
    Node *input_node() const;

  private:
    /** Input node. */
    Node *input_node_;
};
}
