////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/msl_shader_compiler.h"

#include <sstream>
#include <string>

#include "core/colour.h"
#include "core/exception.h"
#include "core/vector3.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/metal/compiler_strings.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/blur_node.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/combine_node.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/composite_node.h"
#include "graphics/render_graph/conditional_node.h"
#include "graphics/render_graph/invert_node.h"
#include "graphics/render_graph/post_processing_node.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/sin_node.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_node.h"
#include "graphics/sampler.h"
#include "graphics/texture.h"
#include "graphics/vertex_attributes.h"

namespace
{

/**
 * Helper function to create a string loading a texture from the global table.
 *
 * @param texture
 *   Texture to create string for.
 *
 * @returns
 *   String loading the texture.
 */
std::string texture_name(const iris::Texture *texture)
{
    return "texture_table[" + std::to_string(texture->index()) + "].texture";
}

/**
 * Helper function to create a string loading a cube map from the global table.
 *
 * @param cube_map
 *   Cube map to create string for.
 *
 * @returns
 *   String loading the cube map.
 */
std::string cube_map_name(const iris::CubeMap *cube_map)
{
    return "cube_map_table[" + std::to_string(cube_map->index()) + "].cube_map";
}

/**
 * Helper function to create a string loading a sampler from the global table.
 *
 * @param sampler
 *   Sampler to create string for.
 *
 * @returns
 *   String loading the sampler.
 */
std::string sampler_name(const iris::Sampler *sampler)
{
    return "sampler_table[" + std::to_string(sampler->index()) + "].smpl";
}

/**
 * Visit a node if it exists or write out a default value to a stream.
 *1
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
 *
 * @param add_semi_colon
 *   If true write semi colon after visit/value, else do nothing.
 */
void visit_or_default(
    std::stringstream &strm,
    const iris::Node *node,
    iris::MSLShaderCompiler *visitor,
    const std::string &default_value,
    bool add_semi_colon = true)
{
    if (node == nullptr)
    {
        strm << default_value;
    }
    else
    {
        node->accept(*visitor);
    }

    if (add_semi_colon)
    {
        strm << ";\n";
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
void build_fragment_colour(std::stringstream &strm, const iris::Node *colour, iris::MSLShaderCompiler *visitor)
{
    strm << "float4 fragment_colour = ";
    visit_or_default(strm, colour, visitor, "in.color");
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
void build_normal(std::stringstream &strm, const iris::Node *normal, iris::MSLShaderCompiler *visitor)
{
    strm << "float3 n = ";
    if (normal == nullptr)
    {
        strm << "normalize(in.normal.xyz);\n";
    }
    else
    {
        strm << "float3(";
        normal->accept(*visitor);
        strm << ");\n";
        strm << "n = normalize(n * 2.0 - 1.0);\n";
    }
}

}

namespace iris
{

MSLShaderCompiler::MSLShaderCompiler(const RenderGraph *render_graph, LightType light_type)
    : vertex_stream_()
    , fragment_stream_()
    , current_stream_(nullptr)
    , vertex_functions_()
    , fragment_functions_()
    , current_functions_(nullptr)
    , light_type_(light_type)
{
    render_graph->render_node()->accept(*this);
}

void MSLShaderCompiler::visit(const RenderNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    // build vertex shader

    vertex_stream_ << vertex_begin;

    if (light_type_ == LightType::DIRECTIONAL)
    {
        *current_stream_ << R"(
        out.frag_pos_light_space = transpose(light_data->proj) * transpose(light_data->view) * out.frag_position;
)";
    }

    *current_stream_ << "return out;";
    *current_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    *current_stream_ << "float2 uv = in.tex.xy;\n";

    build_fragment_colour(fragment_stream_, node.colour_input(), this);
    build_normal(fragment_stream_, node.normal_input(), this);

    // depending on the light type depends on how we interpret the light
    // uniform and how we calculate lighting
    switch (light_type_)
    {
        case LightType::AMBIENT: *current_stream_ << "return light_data->colour * fragment_colour;\n"; break;
        case LightType::DIRECTIONAL:
            *current_stream_ << "float3 light_dir = ";
            *current_stream_
                << (node.normal_input() == nullptr ? "normalize(-light_data->position.xyz);\n"
                                                   : "normalize(-in.tangent_light_pos.xyz);\n");

            *current_stream_ << "float shadow = 0.0;\n";
            *current_stream_
                << "shadow = calculate_shadow(n, in.frag_pos_light_space, light_dir, "
                   "texture_table[shadow_map_index].texture, sampler_table[shadow_map_sampler_index].smpl);\n";

            *current_stream_ << R"(
                float diff = (1.0 - shadow) * max(dot(n, light_dir), 0.0);
                float3 diffuse = float3(diff);
                
                return float4(diffuse * fragment_colour.xyz, 1.0);
                )";
            break;
        case LightType::POINT:
            *current_stream_ << "float3 light_dir = ";
            *current_stream_
                << (node.normal_input() == nullptr ? "normalize(light_data->position.xyz - "
                                                     "in.frag_position.xyz);\n"
                                                   : "normalize(in.tangent_light_pos.xyz - "
                                                     "in.tangent_frag_pos.xyz);\n");
            *current_stream_ << R"(
                float distance  = length(light_data->position.xyz - in.frag_position.xyz);
                float constant_term = light_data->attenuation.x;
                float linear = light_data->attenuation.y;
                float quadratic = light_data->attenuation.z;
                float attenuation = 1.0 / (constant_term + linear * distance + quadratic * (distance * distance));    
                float3 att = float3(attenuation, attenuation, attenuation);

                float diff = max(dot(n, light_dir), 0.0);
                float3 diffuse = float3(diff, diff, diff);
                
                return float4(diffuse * light_data->colour.xyz * fragment_colour.xyz * att, 1.0);
                )";
            break;
    }
}

void MSLShaderCompiler::visit(const PostProcessingNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    // build vertex shader

    vertex_stream_ << vertex_begin;
    *current_stream_ << "return out;";
    *current_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    // build fragment shader

    *current_stream_ << "float2 uv = in.tex.xy;\n";

    build_fragment_colour(fragment_stream_, node.colour_input(), this);

    *current_stream_ << R"(
        float3 mapped = fragment_colour.rgb / (fragment_colour.rgb + float3(1.0, 1.0, 1.0));
        mapped = pow(mapped, float3(1.0 / 2.2));

        return float4(mapped, 1.0);
    )";
}

void MSLShaderCompiler::visit(const SkyBoxNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    // build vertex shader

    *current_stream_ << R"(
        VertexOut out;

        float4x4 adj_view = transpose(camera_data->view);
        adj_view[3] = 0.0f;
        adj_view[3][3] = 1.0f;
        
        out.tex = vertices[vid].position;
        out.position = float4(transpose(camera_data->projection) * adj_view * out.tex).xyww;
    )";

    *current_stream_ << "return out;";
    *current_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    // build fragment shader

    build_fragment_colour(fragment_stream_, node.colour_input(), this);

    *current_stream_ << "float3 tex_coords = float3(in.tex.x, in.tex.y, -in.tex.z);\n"
                     << "return " << cube_map_name(node.sky_box()) << ".sample("
                     << sampler_name(node.sky_box()->sampler()) << ", tex_coords);\n";
}

void MSLShaderCompiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "float4(" << colour.r << ", " << colour.g << ", " << colour.b << ", " << colour.a << ")";
}

void MSLShaderCompiler::visit(const TextureNode &node)
{
    current_functions_->emplace(R"(
float4 sample_texture(texture2d<float> texture, float2 coord, sampler s)
{
    return texture.sample(s, coord);
}
)");

    *current_stream_ << "sample_texture(" << texture_name(node.texture());

    if (node.texture()->flip())
    {
        *current_stream_ << ", float2(uv.x, -uv.y), ";
    }
    else
    {
        *current_stream_ << ", uv, ";
    }

    *current_stream_ << sampler_name(node.texture()->sampler()) << ")";
}

void MSLShaderCompiler::visit(const InvertNode &node)
{
    current_functions_->emplace(invert_function);

    *current_stream_ << "invert(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void MSLShaderCompiler::visit(const BlurNode &node)
{
    current_functions_->emplace(blur_function);

    *current_stream_ << "blur(" << texture_name(node.input_node()->texture());

    if (node.input_node()->texture()->flip())
    {
        *current_stream_ << ", float2(uv.x, -uv.y)";
    }
    else
    {
        *current_stream_ << ", uv";
    }

    *current_stream_ << ", " << sampler_name(node.input_node()->texture()->sampler()) << ")";
}

void MSLShaderCompiler::visit(const CompositeNode &node)
{
    current_functions_->emplace(composite_function);

    *current_stream_ << "composite(";
    node.colour1()->accept(*this);
    *current_stream_ << ", ";
    node.colour2()->accept(*this);
    *current_stream_ << ", ";
    node.depth1()->accept(*this);
    *current_stream_ << ", ";
    node.depth2()->accept(*this);
    *current_stream_ << ", uv)";
}

void MSLShaderCompiler::visit(const ValueNode<float> &node)
{
    *current_stream_ << std::to_string(node.value());
}

void MSLShaderCompiler::visit(const ValueNode<Vector3> &node)
{
    *current_stream_ << "float3(" << node.value().x << ", " << node.value().y << ", " << node.value().z << ")";
}

void MSLShaderCompiler::visit(const ValueNode<Colour> &node)
{
    *current_stream_ << "float4(" << node.value().r << ", " << node.value().g << ", " << node.value().b << ", "
                     << node.value().a << ")";
}

void MSLShaderCompiler::visit(const ArithmeticNode &node)
{
    *current_stream_ << "(";
    if (node.arithmetic_operator() == ArithmeticOperator::DOT)
    {
        *current_stream_ << "dot(";
        node.value1()->accept(*this);
        *current_stream_ << ", ";
        node.value2()->accept(*this);
        *current_stream_ << ")";
    }
    else
    {
        node.value1()->accept(*this);
        switch (node.arithmetic_operator())
        {
            case ArithmeticOperator::ADD: *current_stream_ << " + "; break;
            case ArithmeticOperator::SUBTRACT: *current_stream_ << " - "; break;
            case ArithmeticOperator::MULTIPLY: *current_stream_ << " * "; break;
            case ArithmeticOperator::DIVIDE: *current_stream_ << " / "; break;
            default: throw Exception("unknown arithmetic operator");
        }
        node.value2()->accept(*this);
    }

    *current_stream_ << ")";
}

void MSLShaderCompiler::visit(const ConditionalNode &node)
{
    *current_stream_ << "(";
    node.input_value1()->accept(*this);

    switch (node.conditional_operator())
    {
        case ConditionalOperator::GREATER: *current_stream_ << " > "; break;
    }

    node.input_value2()->accept(*this);

    *current_stream_ << " ? ";
    node.output_value1()->accept(*this);
    *current_stream_ << " : ";
    node.output_value2()->accept(*this);
    *current_stream_ << ")";
}

void MSLShaderCompiler::visit(const ComponentNode &node)
{
    node.input_node()->accept(*this);
    *current_stream_ << "." << node.component();
}

void MSLShaderCompiler::visit(const CombineNode &node)
{
    *current_stream_ << "float4(";
    node.value1()->accept(*this);
    *current_stream_ << ", ";
    node.value2()->accept(*this);
    *current_stream_ << ", ";
    node.value3()->accept(*this);
    *current_stream_ << ", ";
    node.value4()->accept(*this);
    *current_stream_ << ")";
}

void MSLShaderCompiler::visit(const SinNode &node)
{
    *current_stream_ << "sin(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void MSLShaderCompiler::visit(const VertexNode &node)
{
    switch (node.vertex_data_type())
    {
        case VertexDataType::POSITION: *current_stream_ << "in.vertex_position"; break;
        case VertexDataType::NORMAL: *current_stream_ << "in.normal"; break;
        default: throw Exception("unknown vertex data type operator");
    }

    *current_stream_ << node.swizzle().value_or("");
}

std::string MSLShaderCompiler::vertex_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << vertex_in << '\n';
    stream << vertex_out << '\n';
    stream << bone_data_struct << '\n';
    stream << camera_data_struct << '\n';
    stream << light_data_struct << '\n';
    stream << model_data_struct << '\n';

    for (const auto &function : vertex_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
 vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant BoneData *bone_data [[buffer(1)]],
    constant CameraData *camera_data [[buffer(2)]],
    constant LightData *light_data [[buffer(3)]],
    constant ModelData *model_data [[buffer(4)]],
    uint vid [[vertex_id]],
    uint instance_id [[instance_id]]) {)";

    stream << vertex_stream_.str() << '\n';

    return stream.str();
}

std::string MSLShaderCompiler::fragment_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << vertex_in << '\n';
    stream << vertex_out << '\n';
    stream << camera_data_struct << '\n';
    stream << light_data_struct << '\n';
    stream << texture_struct << '\n';
    stream << cube_map_struct << '\n';
    stream << sampler_struct << '\n';

    for (const auto &function : fragment_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    constant CameraData *camera_data [[buffer(0)]],
    constant LightData *light_data [[buffer(1)]],
    device Texture *texture_table [[buffer(2)]],
    device CubeMap *cube_map_table [[buffer(3)]],
    constant int &shadow_map_index [[buffer(4)]],
    device Sampler *sampler_table [[buffer(5)]],
    constant int &shadow_map_sampler_index [[buffer(6)]])
    {
)";

    stream << fragment_stream_.str() << '\n';
    stream << "}\n";

    return stream.str();
}

}
