#pragma once

namespace iris
{

// forward declaration
class ShaderCompiler;

/**
 * This is an interface for Node, which forms part of a render graph. Node
 * implementations can be connected together to describe whats a shader should
 * do.
 */
class Node
{
  public:
    virtual ~Node() = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    virtual void accept(ShaderCompiler &compiler) const = 0;
};
}
