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

/**
 * Implementation of Node which takes four inputs to create a single
 * 4-dimensional value.
 */
class CombineNode : public Node
{
  public:
    /**
     * Create a new CombineNode.
     *
     * @param value1
     *   First value.
     *
     * @param value2
     *   Second value.
     *
     * @param value3
     *   Third value.
     *
     * @param value4
     *   Fourth value.
     */
    CombineNode(Node *value1, Node *value2, Node *value3, Node *value4);

    ~CombineNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get first value.
     *
     * @returns
     *   First value.
     */
    Node *value1() const;

    /**
     * Get second value.
     *
     * @returns
     *   Second value.
     */
    Node *value2() const;

    /**
     * Get third value.
     *
     * @returns
     *   Third value.
     */
    Node *value3() const;

    /**
     * Get fourth value.
     *
     * @returns
     *   Fourth value.
     */
    Node *value4() const;

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

    /** Third value. */
    Node *value3_;

    /** Fourth value. */
    Node *value4_;
};
}
