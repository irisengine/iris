////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "graphics/render_graph/node.h"

namespace iris
{
class ShaderCompiler;

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
    void accept(ShaderCompiler &compiler) const override;

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

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Input node. */
    Node *input_node_;

    /** Component string. */
    std::string component_;
};
}
