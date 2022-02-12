////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <functional>

namespace iris
{

// forward declaration
class ShaderCompiler;

/**
 * This is an interface for Node, which forms part of a render graph. Node
 * implementations can be connected together to describe what a shader should
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

    /**
     * Compute hash of node. Implementations should combine hashes of any child nodes.
     *
     * @return
     *   Hash of node.
     */
    virtual std::size_t hash() const = 0;
};
}

// specialise std::hash for node pointer
namespace std
{

template <>
struct hash<iris::Node *>
{
    size_t operator()(const iris::Node *node) const
    {
        return node == nullptr ? std::hash<nullptr_t>{}(nullptr) : node->hash();
    }
};

}
