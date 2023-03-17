////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>

#include "graphics/render_graph/node.h"

namespace iris
{

class ShaderCompiler;

enum class UnaryOperator
{
    NEGATE,
    NORMALISE,
    SIN,
    COS,
    SQUARE_ROOT
};

/**
 * Implementation of Node which performs a unary operator on an input.
 */
class UnaryOperatorNode : public Node
{
  public:
    /**
     * Create a new UnaryOperatorNode.
     *
     * @param input_node
     *   Input value for operator.
     *
     * @param unary_operator
     *   Operator to apply to input.
     */
    UnaryOperatorNode(Node *input_node, UnaryOperator unary_operator);

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

    /**
     * Get unary operator.
     *
     * @returns
     *   Unary operator.
     */
    UnaryOperator unary_operator() const;

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

    /** Unary operator to apply to input. */
    UnaryOperator unary_operator_;
};
}
