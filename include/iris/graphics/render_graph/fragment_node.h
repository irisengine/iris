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
 * Enumeration of types of fragment data that can be used.
 */
enum class FragmentDataType : std::uint8_t
{
    POSITION,
    VIEW_POSITION,
    FRAGMENT_POSITION,
    TEX_COORD,
    NORMAL
};

/**
 * Implementation of Node for getting fragment data.
 */
class FragmentNode : public Node
{
  public:
    /**
     * Construct a new FragmentNode
     *
     * @param fragment_data_type
     *   The type of fragment data to get.
     */
    FragmentNode(FragmentDataType fragment_data_type);

    /**
     * Construct a new FragmentNode
     *
     * @param fragment_data_type
     *   The type of fragment data to get.
     *
     * @param swizzle
     *   A swizzle to apply to the fragment data.
     */
    FragmentNode(FragmentDataType fragment_data_type, std::string_view swizzle);

    ~FragmentNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     */
    FragmentDataType fragment_data_type() const;

    /**
     * Get swizzle.
     *
     * @return
     *   Swizzle, if one was set.
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
    /** Type of fragment data to get. */
    FragmentDataType fragment_data_type_;

    /** Optional swizzle. */
    std::optional<std::string> swizzle_;
};

}
