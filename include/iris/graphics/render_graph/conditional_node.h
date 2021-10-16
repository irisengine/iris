////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "graphics/render_graph/node.h"

namespace iris
{

class ShaderCompiler;

enum class ConditionalOperator : std::uint8_t
{
    GREATER
};

/**
 * Implementation of Node which performs a conditional operation on two inputs
 * and selects one of two outputs based on the result.
 *
 * This can be summarised as:
 *  (input1 ConditionalOperator input2) ? output1 : output2
 */
class ConditionalNode : public Node
{
  public:
    /**
     * Create a new ConditionalNode.
     *
     * @param input_value1
     *   First input value (left side of operator).
     *
     * @param input_value2
     *   Second input value (right side of operator).
     *
     * @param output_value1
     *   First output value (if conditional is true).
     *
     * @param output_value2
     *   Second output value (if conditional is false).
     *
     * @param conditional_operator
     *   Conditional operator to use with inputs.
     */
    ConditionalNode(
        Node *input_value1,
        Node *input_value2,
        Node *output_value1,
        Node *output_value2,
        ConditionalOperator conditional_operator);

    ~ConditionalNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get input_value1.
     *
     * @returns
     *   input_value1.
     */
    Node *input_value1() const;

    /**
     * Get input_value2.
     *
     * @returns
     *   input_value2.
     */
    Node *input_value2() const;

    /**
     * Get output_value1.
     *
     * @returns
     *   output_value1.
     */
    Node *output_value1() const;

    /**
     * Get output_value2.
     *
     * @returns
     *   output_value2.
     */
    Node *output_value2() const;

    /**
     * Get conditional operator.
     *
     * @returns
     *   Conditional operator.
     */
    ConditionalOperator conditional_operator() const;

  private:
    /** First input value. */
    Node *input_value1_;

    /** Second input value. */
    Node *input_value2_;

    /** First output value. */
    Node *output_value1_;

    /** Second output value. */
    Node *output_value2_;

    /** Conditional operator to use with inputs. */
    ConditionalOperator conditional_operator_;
};
}
