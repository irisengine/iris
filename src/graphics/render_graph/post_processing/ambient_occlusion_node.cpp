////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/post_processing/ambient_occlusion_node.h"

#include <cstddef>

#include "graphics/post_processing_description.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/render_graph/texture_node.h"

namespace iris
{

AmbientOcclusionNode::AmbientOcclusionNode(
    TextureNode *input,
    TextureNode *normal_texture,
    TextureNode *position_texture,
    AmbientOcclusionDescription description)
    : RenderNode()
    , normal_texture_(normal_texture)
    , position_texture_(position_texture)
    , description_(description)
{
    set_colour_input(input);
}

void AmbientOcclusionNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

TextureNode *AmbientOcclusionNode::normal_texture() const
{
    return normal_texture_;
}

TextureNode *AmbientOcclusionNode::position_texture() const
{
    return position_texture_;
}

AmbientOcclusionDescription AmbientOcclusionNode::description() const
{
    return description_;
}

std::size_t AmbientOcclusionNode::hash() const
{
    return combine_hash(colour_input_, "ambient_occlusion_node");
}

}
