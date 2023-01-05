////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/camera_node.h"

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include "core/utils.h"
#include "graphics/render_graph/node.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

CameraNode::CameraNode(CameraDataType camera_data_type)
    : camera_data_type_(camera_data_type)
    , swizzle_(std::nullopt)
{
}

CameraNode::CameraNode(CameraDataType camera_data_type, std::string_view swizzle)
    : camera_data_type_(camera_data_type)
    , swizzle_(swizzle)
{
}

void CameraNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

CameraDataType CameraNode::camera_data_type() const
{
    return camera_data_type_;
}

std::optional<std::string> CameraNode::swizzle() const
{
    return swizzle_;
}

std::size_t CameraNode::hash() const
{
    return combine_hash(camera_data_type_, swizzle_, "camera_node");
}

}
