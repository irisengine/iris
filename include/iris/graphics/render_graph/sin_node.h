#pragma once

#include <memory>

#include "graphics/render_graph/node.h"

namespace iris
{
class ShaderCompiler;

/**
 * Implementation of Node which calculates the sine of the input node.
 */
class SinNode : public Node
{
  public:
    /**
     * Create a new SinNode.
     *
     * @param input_node
     *   Node to sine.
     */
    SinNode(Node *input_node);

    ~SinNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

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
