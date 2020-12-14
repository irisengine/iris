#pragma once

#include "graphics/render_graph/compiler.h"
#include "graphics/render_graph/node.h"

namespace iris
{
/**
 * Implementation of Node which provides access to a constant value. See
 * compiler.h for supported types.
 */
template <class T>
class ValueNode : public Node
{
  public:
    /**
     * Create a new ValueNode.
     *
     * @param value
     *   Value to provide access to.
     */
    ValueNode(const T &value)
        : value_(value)
    {
    }

    ~ValueNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(Compiler &compiler) const override
    {
        return compiler.visit(*this);
    }

    /**
     * Get value.
     *
     * @returns
     *   Value.
     */
    T value() const
    {
        return value_;
    }

  private:
    /** Value. */
    T value_;
};
}
