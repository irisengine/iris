////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/hlsl_shader_compiler.h"

#include <iterator>
#include <sstream>
#include <string>

#include "inja/inja.hpp"

#include "hlsl/ambient_occlusion_node_fragment.h"
#include "hlsl/ambient_occlusion_node_vertex.h"
#include "hlsl/anti_aliasing_node_fragment.h"
#include "hlsl/anti_aliasing_node_vertex.h"
#include "hlsl/arithmetic_node_chunk.h"
#include "hlsl/blur_function.h"
#include "hlsl/blur_node_chunk.h"
#include "hlsl/colour_adjust_node_fragment.h"
#include "hlsl/colour_adjust_node_vertex.h"
#include "hlsl/colour_node_chunk.h"
#include "hlsl/combine_node_chunk.h"
#include "hlsl/component_node_chunk.h"
#include "hlsl/composite_function.h"
#include "hlsl/composite_node_chunk.h"
#include "hlsl/conditional_node_chunk.h"
#include "hlsl/invert_function.h"
#include "hlsl/invert_node_chunk.h"
#include "hlsl/render_node_fragment.h"
#include "hlsl/render_node_vertex.h"
#include "hlsl/rgb_to_luma_function.h"
#include "hlsl/shadow_function.h"
#include "hlsl/sin_node_chunk.h"
#include "hlsl/sky_box_node_fragment.h"
#include "hlsl/sky_box_node_vertex.h"
#include "hlsl/texture_node_chunk.h"
#include "hlsl/value_node_colour_chunk.h"
#include "hlsl/value_node_float_chunk.h"
#include "hlsl/value_node_vector3_chunk.h"
#include "hlsl/vertex_node_chunk.h"

#include "core/colour.h"
#include "core/exception.h"
#include "core/random.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/blur_node.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/combine_node.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/composite_node.h"
#include "graphics/render_graph/conditional_node.h"
#include "graphics/render_graph/invert_node.h"
#include "graphics/render_graph/post_processing/ambient_occlusion_node.h"
#include "graphics/render_graph/post_processing/anti_aliasing_node.h"
#include "graphics/render_graph/post_processing/colour_adjust_node.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/sin_node.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_node.h"
#include "graphics/sampler.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace
{

/**
 * Visit a node if it exists or write out a default value to a stream.
 *
 * This helper function is a little bit brittle as it assumes the visitor will
 * write to the same stream as the one supplied.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param node
 *   Node to visit, if nullptr then default value will be written to stream.
 *
 * @param visitor
 *   Visitor to visit node if not null.
 *
 * @param default_value
 *   Value to write to stream if node not null.
 */
void visit_or_default(
    std::stringstream &strm,
    const iris::Node *node,
    iris::HLSLShaderCompiler *visitor,
    const std::string &default_value)
{
    if (node == nullptr)
    {
        strm << default_value;
    }
    else
    {
        node->accept(*visitor);
    }
}

/**
 * Write shader code for generating fragment colour.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param colour
 *   Node for colour (maybe nullptr).
 *
 * @param visitor
 *   Visitor for node.
 */
void build_fragment_colour(std::stringstream &strm, const iris::Node *colour, iris::HLSLShaderCompiler *visitor)
{
    visit_or_default(strm, colour, visitor, "input.colour");
}

/**
 * Write shader code for generating fragment normal.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param normal
 *   Node for normal (maybe nullptr).
 *
 * @param visitor
 *   Visitor for node.
 */
void build_normal(std::stringstream &strm, const iris::Node *normal, iris::HLSLShaderCompiler *visitor)
{
    if (normal == nullptr)
    {
        strm << "normalize(input.normal.xyz);\n";
    }
    else
    {
        strm << "float3(";
        normal->accept(*visitor);
        strm << ".xyz);\n";
        strm << "normal = normalize(normal * 2.0 - 1.0);\n";
    }
}

}

namespace iris
{
HLSLShaderCompiler::HLSLShaderCompiler(
    const RenderGraph *render_graph,
    LightType light_type,
    bool render_to_normal_target,
    bool render_to_position_target)
    : vertex_stream_()
    , fragment_stream_()
    , fragment_functions_()
    , light_type_(light_type)
    , render_to_normal_target_(render_to_normal_target)
    , render_to_position_target_(render_to_position_target)
{
    render_graph->render_node()->accept(*this);
}

void HLSLShaderCompiler::visit(const RenderNode &node)
{
    // build vertex shader

    const ::inja::json vertex_args{{"is_directional_light", light_type_ == LightType::DIRECTIONAL}};
    vertex_stream_ << ::inja::render(render_node_vertex, vertex_args);

    // build fragment shader

    fragment_functions_.emplace(shadow_function);

    stream_stack_.push({});
    build_fragment_colour(stream_stack_.top(), node.colour_input(), this);
    const std::string fragment_colour = stream_stack_.top().str();
    stream_stack_.pop();

    stream_stack_.push({});
    build_normal(stream_stack_.top(), node.normal_input(), this);
    const auto normal = stream_stack_.top().str();
    stream_stack_.pop();

    ::inja::json fragment_args{
        {"render_normal", render_to_normal_target_},
        {"render_position", render_to_position_target_},
        {"fragment_colour", fragment_colour},
        {"normal", normal},
        {"has_normal", node.normal_input() != nullptr},
        {"light_type", static_cast<std::uint32_t>(light_type_)}};

    if (node.ambient_occlusion_input() != nullptr)
    {
        stream_stack_.push({});
        node.ambient_occlusion_input()->accept(*this);
        fragment_args["ambient_input"] = stream_stack_.top().str();
        stream_stack_.pop();
    }
    else
    {
        fragment_args["ambient_input"] = false;
    }

    fragment_stream_ << ::inja::render(render_node_fragment, fragment_args);
}

void HLSLShaderCompiler::visit(const SkyBoxNode &node)
{
    // build vertex shader

    const ::inja::json vertex_args;
    vertex_stream_ << ::inja::render(sky_box_node_vertex, vertex_args);

    // build fragment shader

    fragment_functions_.emplace(shadow_function);

    const ::inja::json fragment_args{
        {"cube_map_index", node.sky_box()->index()}, {"sampler_index", node.sky_box()->sampler()->index()}};

    fragment_stream_ << ::inja::render(sky_box_node_fragment, fragment_args);
}

void HLSLShaderCompiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();

    const ::inja::json args{{"r", colour.r}, {"g", colour.g}, {"b", colour.b}, {"a", colour.a}};

    stream_stack_.top() << ::inja::render(colour_node_chunk, args);
}

void HLSLShaderCompiler::visit(const TextureNode &node)
{
    const ::inja::json args{
        {"vertex_data", node.uv_source() == UVSource::VERTEX_DATA},
        {"texture_index", node.texture()->index()},
        {"sampler_index", node.texture()->sampler()->index()},
        {"reciprocal_width", 1.0f / node.texture()->width()},
        {"reciprocal_height", 1.0f / node.texture()->height()}};

    stream_stack_.top() << ::inja::render(texture_node_chunk, args);
}

void HLSLShaderCompiler::visit(const InvertNode &node)
{
    fragment_functions_.emplace(invert_function);

    stream_stack_.push({});
    node.input_node()->accept(*this);

    const ::inja::json args{{"input", stream_stack_.top().str()}};

    stream_stack_.pop();
    stream_stack_.top() << ::inja::render(invert_node_chunk, args);
}

void HLSLShaderCompiler::visit(const BlurNode &node)
{
    fragment_functions_.emplace(blur_function);

    const ::inja::json args{
        {"texture_index", node.input_node()->texture()->index()},
        {"sampler_index", node.input_node()->texture()->sampler()->index()}};

    stream_stack_.top() << ::inja::render(blur_node_chunk, args);
}

void HLSLShaderCompiler::visit(const CompositeNode &node)
{
    fragment_functions_.emplace(composite_function);

    std::array<Node *, 4u> nodes{{node.colour1(), node.colour2(), node.depth1(), node.depth2()}};
    std::array<std::string, 4u> node_strs{};

    for (auto i = 0u; i < nodes.size(); ++i)
    {
        stream_stack_.push({});
        nodes[i]->accept(*this);
        node_strs[i] = stream_stack_.top().str();
        stream_stack_.pop();
    }

    const ::inja::json args{
        {"colour1", node_strs[0]}, {"colour2", node_strs[1]}, {"depth1", node_strs[2]}, {"depth2", node_strs[3]}};

    stream_stack_.top() << ::inja::render(composite_node_chunk, args);
}

void HLSLShaderCompiler::visit(const ValueNode<float> &node)
{
    const ::inja::json args{{"value", node.value()}};
    stream_stack_.top() << ::inja::render(value_node_float_chunk, args);
}

void HLSLShaderCompiler::visit(const ValueNode<Vector3> &node)
{
    const auto value = node.value();

    const ::inja::json args{{"x", value.x}, {"y", value.y}, {"z", value.z}};
    stream_stack_.top() << ::inja::render(value_node_vector3_chunk, args);
}

void HLSLShaderCompiler::visit(const ValueNode<Colour> &node)
{
    const auto value = node.value();

    const ::inja::json args{{"r", value.r}, {"g", value.g}, {"b", value.b}, {"a", value.a}};
    stream_stack_.top() << ::inja::render(value_node_colour_chunk, args);
}

void HLSLShaderCompiler::visit(const ArithmeticNode &node)
{
    std::array<Node *, 2u> nodes{{node.value1(), node.value2()}};
    std::array<std::string, 2u> node_strs{};

    for (auto i = 0u; i < nodes.size(); ++i)
    {
        stream_stack_.push({});
        nodes[i]->accept(*this);
        node_strs[i] = stream_stack_.top().str();
        stream_stack_.pop();
    }

    const ::inja::json args{
        {"operator", static_cast<std::uint32_t>(node.arithmetic_operator())},
        {"value1", node_strs[0]},
        {"value2", node_strs[1]}};

    stream_stack_.top() << ::inja::render(arithmetic_node_chunk, args);
}

void HLSLShaderCompiler::visit(const ConditionalNode &node)
{
    std::array<Node *, 4u> nodes{
        {node.input_value1(), node.input_value2(), node.output_value1(), node.output_value2()}};
    std::array<std::string, 4u> node_strs{};

    for (auto i = 0u; i < nodes.size(); ++i)
    {
        stream_stack_.push({});
        nodes[i]->accept(*this);
        node_strs[i] = stream_stack_.top().str();
        stream_stack_.pop();
    }

    const ::inja::json args{
        {"input1", node_strs[0]},
        {"input2", node_strs[1]},
        {"output1", node_strs[2]},
        {"output2", node_strs[3]},
        {"operator", ">"}};

    stream_stack_.top() << ::inja::render(conditional_node_chunk, args);
}

void HLSLShaderCompiler::visit(const ComponentNode &node)
{
    stream_stack_.push({});
    node.input_node()->accept(*this);
    const auto value = stream_stack_.top().str();
    stream_stack_.pop();

    const ::inja::json args{{"value", value, {"component", node.component()}}};

    stream_stack_.top() << ::inja::render(component_node_chunk, args);
}

void HLSLShaderCompiler::visit(const CombineNode &node)
{
    std::array<Node *, 4u> nodes{{node.value1(), node.value2(), node.value3(), node.value4()}};
    std::array<std::string, 4u> node_strs{};

    for (auto i = 0u; i < nodes.size(); ++i)
    {
        stream_stack_.push({});
        nodes[i]->accept(*this);
        node_strs[i] = stream_stack_.top().str();
        stream_stack_.pop();
    }

    const ::inja::json args{{"x", node_strs[0]}, {"y", node_strs[1]}, {"z", node_strs[2]}, {"w", node_strs[3]}};

    stream_stack_.top() << ::inja::render(combine_node_chunk, args);
}

void HLSLShaderCompiler::visit(const SinNode &node)
{
    stream_stack_.push({});
    node.accept(*this);
    const auto value = stream_stack_.top().str();
    stream_stack_.pop();

    const ::inja::json args{{"value", value}};

    stream_stack_.top() << ::inja::render(sin_node_chunk, args);
}

void HLSLShaderCompiler::visit(const VertexNode &node)
{
    const ::inja::json args{
        {"type", static_cast<std::uint32_t>(node.vertex_data_type())}, {"swizzle", node.swizzle().value_or("")}};

    stream_stack_.top() << ::inja::render(vertex_node_chunk, args);
}

void HLSLShaderCompiler::visit(const AmbientOcclusionNode &node)
{
    // build vertex shader

    ::inja::json vertex_args;
    vertex_stream_ << ::inja::render(ambient_occlusion_node_vertex, vertex_args);

    // build fragment shader

    const auto *input_texture = static_cast<const TextureNode *>(node.colour_input())->texture();

    stream_stack_.push({});
    std::string fragment_colour;
    if (node.colour_input() == nullptr)
    {
        fragment_colour = "input.colour";
    }
    else
    {
        node.colour_input()->accept(*this);
        fragment_colour = stream_stack_.top().str();
    }

    const ::inja::json fragment_args{
        {"fragment_colour", fragment_colour},
        {"width", input_texture->width()},
        {"height", input_texture->height()},
        {"position_texture_index", node.position_texture()->texture()->index()},
        {"position_sampler_index", node.position_texture()->texture()->sampler()->index()},
        {"normal_texture_index", node.normal_texture()->texture()->index()},
        {"normal_sampler_index", node.normal_texture()->texture()->sampler()->index()},
        {"sample_count", node.description().sample_count},
        {"radius", node.description().radius},
        {"bias", node.description().bias}};

    fragment_stream_ << ::inja::render(ambient_occlusion_node_fragment, fragment_args);
}

void HLSLShaderCompiler::visit(const ColourAdjustNode &node)
{
    // build vertex shader

    ::inja::json vertex_args;
    vertex_stream_ << ::inja::render(colour_adjust_node_vertex, vertex_args);

    // build fragment shader

    stream_stack_.push({});
    std::string fragment_colour;
    if (node.colour_input() == nullptr)
    {
        fragment_colour = "input.colour";
    }
    else
    {
        node.colour_input()->accept(*this);
        fragment_colour = stream_stack_.top().str();
    }

    const ::inja::json fragment_args{{"fragment_colour", fragment_colour}, {"gamma", 1.0f / node.description().gamma}};
    fragment_stream_ << ::inja::render(colour_adjust_node_fragment, fragment_args);
}

void HLSLShaderCompiler::visit(const AntiAliasingNode &node)
{
    // build vertex shader

    ::inja::json vertex_args;
    vertex_stream_ << ::inja::render(anti_aliasing_node_vertex, vertex_args);

    // build fragment shader

    const auto *input_texture = static_cast<const TextureNode *>(node.colour_input())->texture();

    fragment_functions_.emplace(rgb_to_luma_function);

    stream_stack_.push({});
    std::string fragment_colour;
    if (node.colour_input() == nullptr)
    {
        fragment_colour = "input.colour";
    }
    else
    {
        node.colour_input()->accept(*this);
        fragment_colour = stream_stack_.top().str();
    }

    const ::inja::json fragment_args{
        {"fragment_colour", fragment_colour},
        {"input_texture_index", input_texture->index()},
        {"input_sampler_index", input_texture->sampler()->index()},
        {"inverse_width", 1.0f / static_cast<float>(input_texture->width())},
        {"inverse_height", 1.0f / static_cast<float>(input_texture->height())}};

    fragment_stream_ << ::inja::render(anti_aliasing_node_fragment, fragment_args);
}

std::string HLSLShaderCompiler::vertex_shader() const
{
    std::stringstream strm{};

    strm << vertex_stream_.str() << '\n';

    return strm.str();
}

std::string HLSLShaderCompiler::fragment_shader() const
{
    std::stringstream strm{};

    for (const auto &function : fragment_functions_)
    {
        strm << function << '\n';
    }

    strm << fragment_stream_.str() << '\n';

    return strm.str();
}

}
