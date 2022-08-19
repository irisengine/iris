////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>

#include "graphics/render_graph/node.h"

namespace iris
{

class ShaderCompiler;

enum class ArithmeticOperator : std::uint8_t
{
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    DOT
};

/**
 * Implementation of Node which performs an ArithmeticOperator on two input
 * Nodes.
 *
 * The hierarchy of Arithmetic nodes can be used to set operator precedence,
 * for example:
 *
 *  ValueNode(3) ------\
 *                      ArithmeticNode(+) ------\
 *  ValueNode(4) ------/                         \
 *                                                ArithmeticNode(/)
 *  ValueNode(5) ------\                         /
 *                      ArithmeticNode(+) ------/
 *  ValueNode(6) ------/
 *
 * Weill evaluate to ((3 + 4) / (5 + 6))
 */
class ArithmeticNode : public Node
{
  public:
    /**
     * Create a new ArithmeticNode.
     *
     * @param value1
     *   First input value.
     *
     * @param value2
     *   Second input value.
     *
     * @param arithmetic_operator
     *   Operator to apply to value1 and value2.
     */
    ArithmeticNode(Node *value1, Node *value2, ArithmeticOperator arithmetic_operator);

    ~ArithmeticNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get value1.
     *
     * @returns
     *   value1.
     */
    Node *value1() const;

    /**
     * Get value2.
     *
     * @returns
     *   value2.
     */
    Node *value2() const;

    /**
     * Get arithmetic operator.
     *
     * @returns
     *   Arithmetic operator.
     */
    ArithmeticOperator arithmetic_operator() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** First value. */
    Node *value1_;

    /** Second value. */
    Node *value2_;

    /** Arithmetic operator applied to value1 and value2. */
    ArithmeticOperator arithmetic_operator_;
};
}
