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

/**
 * Implementation of Node for lerpinng - linear interpolating between two values.
 */
class LerpNode : public Node
{
  public:
    /**
     * Create a new LerpNode.
     *
     * @param input_value1
     *   First input value (left side of operator).
     *
     * @param input_value2
     *   Second input value (right side of operator).
     *
     * @param lerp_amount
     *   The amount to interpolate. Should evaluate to a float in the range [0.0, 1.0]
     */
    LerpNode(Node *input_value1, Node *input_value2, Node *lerp_amount);

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
     * Get lerp amount node.
     *
     * @returns
     *   Lerp amount node.
     */
    Node *lerp_amount() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** First input value. */
    Node *input_value1_;

    /** Second input value. */
    Node *input_value2_;

    /** Lerp amount node. */
    Node *lerp_amount_;
};
}
