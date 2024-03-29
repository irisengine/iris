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
 * Implementation of Node which provides access to variables that have been created in the render graph.
 */
class VariableNode : public Node
{
  public:
    /**
     * Create a new VariableNode.
     *
     * @param name
     *   Name of the variable.
     */
    VariableNode(const std::string &name);

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get name of variable.
     *
     * @returns
     *   Variable name.
     */
    std::string name() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Variable name. */
    std::string name_;
};

}
