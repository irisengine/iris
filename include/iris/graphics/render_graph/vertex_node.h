////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "graphics/render_graph/node.h"

namespace iris
{

class ShaderCompiler;

/**
 * Enumeration of types of vertex data that can be used.
 */
enum class VertexDataType : std::uint8_t
{
    POSITION,
    NORMAL,
    UV
};

/**
 * Get vertex data. Note that this will be used in the fragment shader and so will be an interpolated value.
 */
class VertexNode : public Node
{
  public:
    /**
     * Construct a new VertexNode
     *
     * @param vertex_data_type
     *   The type of vertex data to get.
     */
    VertexNode(VertexDataType vertex_data_type);

    /**
     * Construct a new VertexNode
     *
     * @param vertex_data_type
     *   The type of vertex data to get.
     *
     * @param swizzle
     *   A swizzle to apply to the vertex data.
     */
    VertexNode(VertexDataType vertex_data_type, std::string_view swizzle);

    ~VertexNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get arithmetic operator.
     *
     * @returns
     *   Arithmetic operator.
     */
    VertexDataType vertex_data_type() const;

    /**
     * Get swizzle.
     *
     * @return
     *   Swizzle, of one was set.
     */
    std::optional<std::string> swizzle() const;

    /**
     * Compute hash of node.
     *
     * @return
     *   Hash of node.
     */
    std::size_t hash() const override;

  private:
    /** Type of vertex data to get.c */
    VertexDataType vertex_data_type_;

    /** Optional swizzle. */
    std::optional<std::string> swizzle_;
};
}
