////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "graphics/render_graph/node.h"

namespace iris
{

class ShaderCompiler;

/**
 * Implementation of Node for getting a property value. A property is a runtime configurable variable in a shader.
 */
class PropertyNode : public Node
{
  public:
    /**
     * Create a new PropertyNode.
     *
     * @param name
     *   User name of the property.
     */
    PropertyNode(const std::string &name);

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get property name.
     *
     * @returns
     *   User name of property.
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
    /** User name of property. */
    std::string name_;
};

}
