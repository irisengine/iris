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
 * Enumeration of types of camera data that can be used.
 */
enum class CameraDataType : std::uint8_t
{
    POSITION,
};

/**
 * Implementation of Node for getting camera data.
 */
class CameraNode : public Node
{
  public:
    /**
     * Construct a new CameraNode
     *
     * @param camera_data_type
     *   The type of camera data to get.
     */
    CameraNode(CameraDataType camera_data_type);

    /**
     * Construct a new CameraNode
     *
     * @param camera_data_type
     *   The type of camera data to get.
     *
     * @param swizzle
     *   A swizzle to apply to the camera data.
     */
    CameraNode(CameraDataType camera_data_type, std::string_view swizzle);

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     */
    CameraDataType camera_data_type() const;

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
    /** Type of camera data to get. */
    CameraDataType camera_data_type_;

    /** Optional swizzle. */
    std::optional<std::string> swizzle_;
};

}
