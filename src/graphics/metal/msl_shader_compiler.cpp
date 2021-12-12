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
#include "graphics/render_graph/vertex_position_node.h"
#include "graphics/texture.h"
#include "graphics/vertex_attributes.h"

namespace
{

/**
 * Helper function to create a variable name for a texture. Always returns the
 * same name for the same texture.
 *
 * @param texture
 *   Texture to generate name for.
 *
 * @param textures
 *   Collection of textures, texture will be inserted if it does not exist.
 *
 * @returns
 *   Unique variable name for the texture.
 */
std::string texture_name(iris::Texture *texture, std::vector<iris::Texture *> &textures)
{
    std::size_t id = 0u;

    const auto find = std::find(std::cbegin(textures), std::cend(textures), texture);

    if (find != std::cend(textures))
    {
        id = std::distance(std::cbegin(textures), find);
    }
    else
    {
        id = textures.size();
        textures.emplace_back(texture);
    }

    return "tex" + std::to_string(id);
}

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
    , textures_()
    , light_type_(light_type)
    , cube_map_(nullptr)
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
        out.frag_pos_light_space = light_uniform->proj * light_uniform->view * out.frag_position;
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
        case LightType::AMBIENT: *current_stream_ << "return uniform->light_colour * fragment_colour;\n"; break;
        case LightType::DIRECTIONAL:
            *current_stream_ << "float3 light_dir = ";
            *current_stream_
                << (node.normal_input() == nullptr ? "normalize(-uniform->light_position.xyz);\n"
                                                   : "normalize(-in.tangent_light_pos.xyz);\n");

            *current_stream_ << "float shadow = 0.0;\n";
            *current_stream_ <<
                R"(shadow = calculate_shadow(n, in.frag_pos_light_space, light_dir, shadow_map, shadow_sampler);
                )";

            *current_stream_ << R"(
                float diff = (1.0 - shadow) * max(dot(n, light_dir), 0.0);
                float3 diffuse = float3(diff);
                
                return float4(diffuse * fragment_colour.xyz, 1.0);
                )";
            break;
        case LightType::POINT:
            *current_stream_ << "float3 light_dir = ";
            *current_stream_
                << (node.normal_input() == nullptr ? "normalize(uniform->light_position.xyz - "
                                                     "in.frag_position.xyz);\n"
                                                   : "normalize(in.tangent_light_pos.xyz - "
                                                     "in.tangent_frag_pos.xyz);\n");
            *current_stream_ << R"(
                float distance  = length(uniform->light_position.xyz - in.frag_position.xyz);
                float constant_term = uniform->light_attenuation[0];
                float linear = uniform->light_attenuation[1];
                float quadratic = uniform->light_attenuation[2];
                float attenuation = 1.0 / (constant_term + linear * distance + quadratic * (distance * distance));    
                float3 att = float3(attenuation, attenuation, attenuation);

                float diff = max(dot(n, light_dir), 0.0);
                float3 diffuse = float3(diff, diff, diff);
                
                return float4(diffuse * uniform->light_colour.xyz * fragment_colour.xyz * att, 1.0);
                )";
            break;
    }

    *current_stream_ << "}";
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
    })";
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

        float4x4 adj_view = uniform->view;
        adj_view[3] = 0.0f;
        adj_view[3][3] = 1.0f;
        
        out.tex = vertices[vid].position;
        out.position = float4(uniform->projection * adj_view * out.tex).xyww;
    )";

    *current_stream_ << "return out;";
    *current_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    // build fragment shader

    build_fragment_colour(fragment_stream_, node.colour_input(), this);

    *current_stream_ << R"(
        float3 tex_coords = float3(in.tex.x, in.tex.y, -in.tex.z);
        return sky_box.sample(sky_box_sampler, tex_coords);
    })";

    cube_map_ = node.sky_box();
}

void MSLShaderCompiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "float4(" << colour.r << ", " << colour.g << ", " << colour.b << ", " << colour.a << ")";
}

void MSLShaderCompiler::visit(const TextureNode &node)
{
    current_functions_->emplace(R"(
float4 sample_texture(texture2d<float> texture, float2 coord)
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);
    return texture.sample(s, coord);
}
)");

    *current_stream_ << "sample_texture(" << texture_name(node.texture(), textures_);

    if (node.texture()->flip())
    {
        *current_stream_ << ", float2(uv.x, -uv.y))";
    }
    else
    {
        *current_stream_ << ", uv)";
    }
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

    *current_stream_ << "blur(" << texture_name(node.input_node()->texture(), textures_);

    if (node.input_node()->texture()->flip())
    {
        *current_stream_ << ", float2(uv.x, -uv.y))";
    }
    else
    {
        *current_stream_ << ", uv)";
    }
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

void MSLShaderCompiler::visit(const VertexPositionNode &node)
{
    *current_stream_ << "vertices[vid].position";
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
    *current_stream_ << "float4(" << node.value().g << ", " << node.value().g << ", " << node.value().b << ", "
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

std::string MSLShaderCompiler::vertex_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << vertex_in << '\n';
    stream << vertex_out << '\n';
    stream << default_uniform << '\n';
    stream << directional_light_uniform << '\n';
    stream << point_light_uniform << '\n';

    for (const auto &function : vertex_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
 vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant DefaultUniform *uniform [[buffer(1)]],
    constant DirectionalLightUniform *light_uniform [[buffer(2)]],
    uint vid [[vertex_id]])";
    for (auto i = 0u; i < textures_.size(); ++i)
    {
        stream << "    ,texture2d<float> tex" << i << " [[texture(" << i << ")]]" << '\n';
    }
    stream << R"() {)";

    stream << vertex_stream_.str() << '\n';

    return stream.str();
}

std::string MSLShaderCompiler::fragment_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << vertex_in << '\n';
    stream << vertex_out << '\n';
    stream << default_uniform << '\n';
    stream << directional_light_uniform << '\n';
    stream << point_light_uniform << '\n';

    for (const auto &function : fragment_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    constant DefaultUniform *uniform [[buffer(0)]],
    constant DirectionalLightUniform *light_uniform [[buffer(1)]],
    sampler shadow_sampler [[sampler(0)]],
    texture2d<float> shadow_map [[texture(0)]],
    sampler sky_box_sampler [[sampler(1)]],
    texturecube<float> sky_box [[texture(1)]])";

    for (auto i = 0u; i < textures_.size(); ++i)
    {
        stream << "    ,texture2d<float> tex" << i << " [[texture(" << i + 2 << ")]]" << '\n';
    }
    stream << R"(
    )
{
)";

    stream << fragment_stream_.str() << '\n';

    return stream.str();
}

std::vector<Texture *> MSLShaderCompiler::textures() const
{
    return textures_;
}

const CubeMap *MSLShaderCompiler::cube_map() const
{
    return cube_map_;
}

}
