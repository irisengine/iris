////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/hlsl_shader_compiler.h"

#include <iterator>
#include <sstream>
#include <string>

#include "core/colour.h"
#include "core/exception.h"
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
#include "graphics/render_graph/post_processing_node.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/sin_node.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_node.h"
#include "graphics/texture.h"

namespace
{

static constexpr auto uniforms = R"(
cbuffer DefaultUniforms : register(b0)
{
    matrix projection;
    matrix view;
    float4 camera;
    matrix model;
    matrix normal_matrix;
    matrix bones[100];
};

cbuffer Light : register(b1)
{
    matrix light_projection;
    matrix light_view;
    float4 light_colour;
    float4 light_position;
    float4 light_attenuation;
};)";

static constexpr auto ps_input = R"(
struct PSInput
{
    precise float4 position : SV_POSITION;
    precise float4 frag_position : POSITION0;
    precise float4 tangent_view_pos : POSITION1;
    precise float4 tangent_frag_pos : POSITION2;
    precise float4 tangent_light_pos : POSITION3;
    precise float4 frag_pos_light_space : POSITION4;
    precise float4 normal : NORMAL;
    precise float4 colour : COLOR;
    precise float2 tex_coord : TEXCOORD;
};)";

static constexpr auto invert_function = R"(
float4 invert(float4 colour)
{
    return float4(1.0 - colour.r, 1.0 - colour.g, 1.0 - colour.b, colour.a);
})";

static constexpr auto composite_function = R"(
float4 composite(float4 colour1, float4 colour2, float4 depth1, float4 depth2, float2 tex_coord)
{
    float4 colour = colour2;

    if(depth1.r < depth2.r)
    {
        colour = colour1;
    }

    return colour;
})";

static constexpr auto blur_function = R"(
float4 blur(Texture2D tex, float2 tex_coords)
{
    const float offset = 1.0 / 500.0;  
    float2 offsets[9] = {
        float2(-offset,  offset), // top-left
        float2( 0.0f,    offset), // top-center
        float2( offset,  offset), // top-right
        float2(-offset,  0.0f),   // center-left
        float2( 0.0f,    0.0f),   // center-center
        float2( offset,  0.0f),   // center-right
        float2(-offset, -offset), // bottom-left
        float2( 0.0f,   -offset), // bottom-center
        float2( offset, -offset)  // bottom-right    
    };

    float k[9] = {
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0  
    };


    float3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = tex.Sample(g_sampler, tex_coords + offsets[i]);
    }

    float3 col = float3(0.0, 0.0, 0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * k[i];
    }
    return float4(col, 1.0);
})";

static constexpr auto shadow_function = R"(
float calculate_shadow(float3 n, float4 frag_pos_light_space, float3 light_dir, Texture2D tex)
{
    float shadow = 0.0;

    float3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;

    float2 proj_uv = float2(proj_coord.x, -proj_coord.y);
    proj_uv = proj_uv * 0.5 + 0.5;

    float closest_depth = tex.Sample(g_sampler, proj_uv).r;
    float current_depth = proj_coord.z;
    float bias = 0.001;// max(0.05 * (1.0 - dot(n, light_dir)), 0.005);

    shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
    if(proj_coord.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
})";

/**
 * Helper function to create a unique id for a texture. Always returns the
 * same name for the same texture.
 *
 * @param texture
 *   Texture to generate id for.
 *
 * @param textures
 *   Collection of textures, texture will be inserted if it does not exist.
 *
 * @returns
 *   Unique id for the texture.
 */
std::size_t texture_id(const iris::Texture *texture, std::vector<const iris::Texture *> &textures)
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

    return id + 2u;
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
    iris::HLSLShaderCompiler *visitor,
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
void build_fragment_colour(std::stringstream &strm, const iris::Node *colour, iris::HLSLShaderCompiler *visitor)
{
    strm << "float4 fragment_colour = ";
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
    strm << "float3 n = ";
    if (normal == nullptr)
    {
        strm << "normalize(input.normal.xyz);\n";
    }
    else
    {
        strm << "float3(";
        normal->accept(*visitor);
        strm << ".xyz);\n";
        strm << "n = normalize(n * 2.0 - 1.0);\n";
    }
}

}

namespace iris
{
HLSLShaderCompiler::HLSLShaderCompiler(const RenderGraph *render_graph, LightType light_type)
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

void HLSLShaderCompiler::visit(const RenderNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    // build vertex shader

    *current_stream_ << R"(
PSInput main(
    float4 position : TEXCOORD0,
    float4 normal : TEXCOORD1,
    float4 colour : TEXCOORD2,
    float4 tex_coord : TEXCOORD3,
    float4 tangent : TEXCOORD4,
    float4 bitangent : TEXCOORD5,
    uint4 bone_ids : TEXCOORD6,
    float4 bone_weights : TEXCOORD7)
{
    matrix bone_transform = mul(bones[bone_ids[0]], bone_weights[0]);
    bone_transform += mul(bones[bone_ids[1]], bone_weights[1]);
    bone_transform += mul(bones[bone_ids[2]], bone_weights[2]);
    bone_transform += mul(bones[bone_ids[3]], bone_weights[3]);

    float3 T = normalize(mul(mul(bone_transform, tangent), normal_matrix).xyz);
    float3 B = normalize(mul(mul(bone_transform, bitangent), normal_matrix).xyz);
    float3 N = normalize(mul(mul(bone_transform, normal), normal_matrix).xyz);

    float3x3 tbn = transpose(float3x3(T, B, N));

    PSInput result;

    result.frag_position = mul(position, bone_transform);
    result.frag_position = mul(result.frag_position, model);

    result.tangent_light_pos = float4(mul(light_position.xyz, tbn), 0.0);
    result.tangent_view_pos = float4(mul(camera.xyz, tbn), 0.0);
    result.tangent_frag_pos = float4(mul(result.frag_position, tbn), 0.0);
)";
    if (light_type_ == LightType::DIRECTIONAL)
    {
        *current_stream_ << R"(
        result.frag_pos_light_space = mul(result.frag_position, light_view);
        result.frag_pos_light_space = mul(result.frag_pos_light_space, light_projection);
)";
    }
    *current_stream_ << R"(
    result.position = mul(result.frag_position, view);
    result.position = mul(result.position, projection);
    result.normal = mul(normal, bone_transform);
    result.normal = mul(result.normal, normal_matrix);
    result.colour = colour;
    result.tex_coord = tex_coord;

    return result;
})";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    *current_stream_ << R"(
float4 main(PSInput input) : SV_TARGET
{)";

    build_fragment_colour(*current_stream_, node.colour_input(), this);
    build_normal(*current_stream_, node.normal_input(), this);

    // depending on the light type depends on how we interpret the light
    // constant data and how we calculate lighting
    switch (light_type_)
    {
        case LightType::AMBIENT: *current_stream_ << "return light_colour * fragment_colour;"; break;
        case LightType::DIRECTIONAL:
            *current_stream_ << "float3 light_dir = ";
            *current_stream_
                << (node.normal_input() == nullptr ? "normalize(-light_position.xyz);\n"
                                                   : "normalize(-input.tangent_light_pos.xyz);\n");

            *current_stream_ << "float shadow = 0.0;\n";
            *current_stream_ <<
                R"(shadow = calculate_shadow(n, input.frag_pos_light_space, light_dir, g_shadow_map);
                )";

            *current_stream_ << R"(
                float diff = (1.0 - shadow) * max(dot(n, light_dir), 0.0);
                float3 diffuse = {diff, diff, diff};
                
                return float4(diffuse * fragment_colour, 1.0);
                )";
            break;
        case LightType::POINT:
            *current_stream_ << "float3 light_dir = ";
            *current_stream_
                << (node.normal_input() == nullptr ? "normalize(light_position.xyz - "
                                                     "input.frag_position.xyz);\n"
                                                   : "normalize(input.tangent_light_pos.xyz - "
                                                     "input.tangent_frag_pos.xyz);\n");
            *current_stream_ << R"(
                float distance  = length(light_position.xyz - input.frag_position.xyz);
                float constant = light_attenuation.x;
                float linear_term = light_attenuation.y;
                float quadratic = light_attenuation.z;
                float attenuation = 1.0 / (constant + linear_term * distance + quadratic * (distance * distance));    
                float3 att = {attenuation, attenuation, attenuation};

                float diff = max(dot(n, light_dir), 0.0);
                float3 diffuse = {diff, diff, diff};
                
                return float4(diffuse * light_colour.xyz * fragment_colour.xyz * att, 1.0);
                )";
            break;
        default: throw Exception("unknown light type");
    }

    *current_stream_ << "}";
}

void HLSLShaderCompiler::visit(const PostProcessingNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    // build vertex shader

    *current_stream_ << R"(
PSInput main(
    float4 position : TEXCOORD0,
    float4 normal : TEXCOORD1,
    float4 colour : TEXCOORD2,
    float4 tex_coord : TEXCOORD3,
    float4 tangent : TEXCOORD4,
    float4 bitangent : TEXCOORD5,
    uint4 bone_ids : TEXCOORD6,
    float4 bone_weights : TEXCOORD7)
{
    matrix bone_transform = mul(bones[bone_ids[0]], bone_weights[0]);
    bone_transform += mul(bones[bone_ids[1]], bone_weights[1]);
    bone_transform += mul(bones[bone_ids[2]], bone_weights[2]);
    bone_transform += mul(bones[bone_ids[3]], bone_weights[3]);

    PSInput result;

    result.frag_position = mul(position, bone_transform);
    result.frag_position = mul(result.frag_position, model);
    result.position = mul(result.frag_position, view);
    result.position = mul(result.position, projection);
    result.colour = colour;
    result.tex_coord = tex_coord;

    return result;
})";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    *current_stream_ << R"(
float4 main(PSInput input) : SV_TARGET
{)";

    build_fragment_colour(*current_stream_, node.colour_input(), this);

    *current_stream_ << R"(
        float3 mapped = fragment_colour.rgb / (fragment_colour.rgb + float3(1.0, 1.0, 1.0));
        mapped = pow(mapped, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

        return float4(mapped.r, mapped.g, mapped.b, 1.0);
    })";
}

void HLSLShaderCompiler::visit(const SkyBoxNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    // build vertex shader

    *current_stream_ << R"(
PSInput main(
    float4 position : TEXCOORD0,
    float4 normal : TEXCOORD1,
    float4 colour : TEXCOORD2,
    float4 tex_coord : TEXCOORD3,
    float4 tangent : TEXCOORD4,
    float4 bitangent : TEXCOORD5,
    uint4 bone_ids : TEXCOORD6,
    float4 bone_weights : TEXCOORD7)
{
    PSInput result;

    float4x4 adj_view = view;
    adj_view[3][0] = 0.0f;
    adj_view[3][1] = 0.0f;
    adj_view[3][2] = 0.0f;
    adj_view[3][3] = 1.0f;

    result.normal = position;
    result.position = mul(position, adj_view);
    result.position = mul(result.position, projection);
    result.position = result.position.xyww;

    return result;
})";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    *current_stream_ << R"(
float4 main(PSInput input) : SV_TARGET
{)";

    build_fragment_colour(*current_stream_, node.colour_input(), this);

    *current_stream_ << R"(
        return g_sky_box.SampleLevel(g_sampler, normalize(input.normal.xyz), 0).rgba;
    })";

    cube_map_ = node.sky_box();
}

void HLSLShaderCompiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "float4(" << colour.r << ", " << colour.g << ", " << colour.b << ", " << colour.a << ")";
}

void HLSLShaderCompiler::visit(const TextureNode &node)
{
    const auto id = texture_id(node.texture(), textures_);

    *current_stream_ << "g_texture" << id << ".Sample(g_sampler,";

    if (node.texture()->flip())
    {
        *current_stream_ << "float2(input.tex_coord.x, 1.0 - input.tex_coord.y))";
    }
    else
    {
        *current_stream_ << " input.tex_coord)";
    }
}

void HLSLShaderCompiler::visit(const InvertNode &node)
{
    current_functions_->emplace(invert_function);

    *current_stream_ << "invert(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void HLSLShaderCompiler::visit(const BlurNode &node)
{
    const auto id = texture_id(node.input_node()->texture(), textures_);

    current_functions_->emplace(blur_function);

    *current_stream_ << "blur(g_texture" << id << ",";
    if (node.input_node()->texture()->flip())
    {
        *current_stream_ << "float2(input.tex_coord.x, 1.0 - input.tex_coord.y))";
    }
    else
    {
        *current_stream_ << " input.tex_coord)";
    }
}

void HLSLShaderCompiler::visit(const CompositeNode &node)
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
    *current_stream_ << ", input.tex_coord)";
}

void HLSLShaderCompiler::visit(const ValueNode<float> &node)
{
    *current_stream_ << std::to_string(node.value());
}

void HLSLShaderCompiler::visit(const ValueNode<Vector3> &node)
{
    *current_stream_ << "float3(" << node.value().x << ", " << node.value().y << ", " << node.value().z << ")";
}

void HLSLShaderCompiler::visit(const ValueNode<Colour> &node)
{
    *current_stream_ << "float4(" << node.value().g << ", " << node.value().g << ", " << node.value().b << ", "
                     << node.value().a << ")";
}

void HLSLShaderCompiler::visit(const ArithmeticNode &node)
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

void HLSLShaderCompiler::visit(const ConditionalNode &node)
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

void HLSLShaderCompiler::visit(const ComponentNode &node)
{
    node.input_node()->accept(*this);
    *current_stream_ << "." << node.component();
}

void HLSLShaderCompiler::visit(const CombineNode &node)
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

void HLSLShaderCompiler::visit(const SinNode &node)
{
    *current_stream_ << "sin(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void HLSLShaderCompiler::visit(const VertexNode &node)
{
    switch (node.vertex_data_type())
    {
        case VertexDataType::POSITION: *current_stream_ << "input.vertex_position"; break;
        case VertexDataType::NORMAL: *current_stream_ << "input.normal"; break;
        default: throw Exception("unknown vertex data type operator");
    }

    *current_stream_ << node.swizzle().value_or("");
}

std::string HLSLShaderCompiler::vertex_shader() const
{
    std::stringstream strm{};

    for (const auto &function : vertex_functions_)
    {
        strm << function << '\n';
    }

    strm << uniforms << '\n';
    strm << ps_input << '\n';

    strm << vertex_stream_.str() << '\n';

    return strm.str();
}

std::string HLSLShaderCompiler::fragment_shader() const
{
    std::stringstream strm{};

    strm << "SamplerState g_sampler : register(s0);\n";
    strm << "Texture2D g_shadow_map : register(t0);\n";
    strm << "TextureCube g_sky_box : register(t1);\n";

    for (auto i = 0u; i < textures_.size(); ++i)
    {
        strm << "Texture2D g_texture" << i + 2u << " : register(t" << i + 2u << ");\n";
    }

    for (const auto &function : fragment_functions_)
    {
        strm << function << '\n';
    }

    strm << uniforms << '\n';
    strm << ps_input << '\n';
    strm << fragment_stream_.str() << '\n';

    return strm.str();
}

}
