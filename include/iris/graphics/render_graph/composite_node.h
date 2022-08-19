////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>

#include "core/vector3.h"
#include "graphics/render_graph/node.h"

namespace iris
{
class ShaderCompiler;

/**
 * Implementation of Node that composites two other nodes together. For each
 * fragment colour1 or colour2 is picked based upon which has the closest depth
 * value.
 */
class CompositeNode : public Node
{
  public:
    /**
     * Create a new CompositeNode.
     *
     * @param colour1
     *   First colour node.
     *
     * @param colour2
     *   Second colour node.
     *
     * @param depth1
     *   Depth values for colour1.
     *
     * @param depth2
     *   Depth values for colour2.
     */
    CompositeNode(Node *colour1, Node *colour2, Node *depth1, Node *depth2);

    ~CompositeNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get first colour.
     *
     * @returns
     *   First colour.
     */
    Node *colour1() const;

    /**
     * Get second colour.
     *
     * @returns
     *   Second colour.
     */
    Node *colour2() const;

    /**
     * Get first depth.
     *
     * @returns
     *   First depth.
     */
    Node *depth1() const;

    /**
     * Get second depth.
     *
     * @returns
     *   Second depth.
     */
    Node *depth2() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** First colour. */
    Node *colour1_;

    /** Second colour. */
    Node *colour2_;

    /** First depth. */
    Node *depth1_;

    /** Second depth. */
    Node *depth2_;
};
}
