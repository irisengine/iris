#pragma once

#include <memory>
#include <string>

#include "graphics/render_graph/node.h"

namespace iris
{
class Compiler;

/**
 * Implementation of Node for extracting components from an input_node.
 */
class ComponentNode : public Node
{
  public:
    /**
     * Create a new ComponentNode.
     *
     * @param input_node
     *   Node to get component from.
     *
     * @param component
     *   String representation of components, supports swizzling e.g. "x", "xy",
     *   "rgb".
     */
    ComponentNode(Node *input_node, const std::string &component);

    ~ComponentNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(Compiler &compiler) const override;

    /**
     * Get input_node node.
     *
     * @returns
     *   Input node.
     */
    Node *input_node() const;

    /**
     * Get component string.
     *
     * @returns
     *   Component string.
     */
    std::string component() const;

  private:
    /** Input node. */
    Node *input_node_;

    /** Component string. */
    std::string component_;
};
}
