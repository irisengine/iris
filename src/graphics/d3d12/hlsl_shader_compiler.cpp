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

static constexpr auto uniforms = R"(
cbuffer BoneData : register(b0)
{
    matrix bones[100];
};

cbuffer Light : register(b1)
{
    matrix light_projection;
    matrix light_view;
    float4 light_colour;
    float4 light_position;
    float4 light_attenuation;
};

cbuffer CameraData : register(b2)
{
    matrix projection;
    matrix view;
    float4 camera;
};

struct ShadowMapIndex
{
    int index;
};

ConstantBuffer<ShadowMapIndex> shadow_map_index : register(b3);

ConstantBuffer<ShadowMapIndex> shadow_map_sampler_index : register(b4);

struct ModelData
{
    matrix model;
    matrix normal_matrix;
};

StructuredBuffer<ModelData> model_data : register(t0);

)";

static constexpr auto ps_input = R"(
struct PSInput
{
    precise float4 position : SV_POSITION;
    precise float4 vertex_position : POSITION0;
    precise float4 frag_position : POSITION1;
    precise float4 tangent_view_pos : POSITION2;
    precise float4 tangent_frag_pos : POSITION3;
    precise float4 tangent_light_pos : POSITION4;
    precise float4 frag_pos_light_space : POSITION5;
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
float4 blur(Texture2D tex, float2 tex_coords, SamplerState smpler)
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
        sampleTex[i] = tex.Sample(smpler, tex_coords + offsets[i]);
    }

    float3 col = float3(0.0, 0.0, 0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * k[i];
    }
    return float4(col, 1.0);
})";

static constexpr auto shadow_function = R"(
float calculate_shadow(float3 n, float4 frag_pos_light_space, float3 light_dir, Texture2D tex, SamplerState smpler)
{
    float shadow = 0.0;

    float3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;

    float2 proj_uv = float2(proj_coord.x, -proj_coord.y);
    proj_uv = proj_uv * 0.5 + 0.5;

    float closest_depth = tex.Sample(smpler, proj_uv).r;
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
 * Helper function to create a string loading a texture from the global table.
 *
 * @param texture
 *   Texture to generate string for.
 *
 * @returns
 *   String loading the texture.
 */
std::string texture_name(const iris::Texture *texture)
{
    return "texture_table[" + std::to_string(texture->index()) + "]";
}

/**
 * Helper function to create a string loading a cube map from the global table.
 *
 * @param cube_map
 *   CubeMap to generate string for.
 *
 * @returns
 *   String loading the cube map.
 */
std::string cube_map_name(const iris::CubeMap *cube_map)
{
    return "cube_map_table[" + std::to_string(cube_map->index()) + "]";
}

/**
 * Helper function to create a string loading a sampler from the global table.
 *
 * @param sampler
 *   Sampler to generate string for.
 *
 * @returns
 *   String loading the sampler.
 */
std::string sampler_name(const iris::Sampler *sampler = nullptr)
{
    return "sampler_table[" + std::to_string(sampler->index()) + "]";
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
    , light_type_(light_type)
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
    float4 bone_weights : TEXCOORD7,
    uint instance_id : SV_InstanceID)
{
    matrix bone_transform = mul(bones[bone_ids[0]], bone_weights[0]);
    bone_transform += mul(bones[bone_ids[1]], bone_weights[1]);
    bone_transform += mul(bones[bone_ids[2]], bone_weights[2]);
    bone_transform += mul(bones[bone_ids[3]], bone_weights[3]);

    float3 T = normalize(mul(mul(bone_transform, tangent), model_data[instance_id].normal_matrix).xyz);
    float3 B = normalize(mul(mul(bone_transform, bitangent), model_data[instance_id].normal_matrix).xyz);
    float3 N = normalize(mul(mul(bone_transform, normal), model_data[instance_id].normal_matrix).xyz);

    float3x3 tbn = transpose(float3x3(T, B, N));

    PSInput result;

    result.frag_position = mul(position, bone_transform);
    result.frag_position = mul(result.frag_position, model_data[instance_id].model);

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
    result.vertex_position = position;
    result.position = mul(result.frag_position, view);
    result.position = mul(result.position, projection);
    result.normal = mul(normal, bone_transform);
    result.normal = mul(result.normal, model_data[instance_id].normal_matrix);
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
            *current_stream_
                << "shadow = calculate_shadow(n, input.frag_pos_light_space, light_dir, "
                   "texture_table[shadow_map_index.index], sampler_table[shadow_map_sampler_index.index]);";

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

    *current_stream_ << "return " << cube_map_name(node.sky_box()) << ".SampleLevel("
                     << sampler_name(node.sky_box()->sampler()) << ", normalize(input.normal.xyz), 0).rgba;\n}";
}

void HLSLShaderCompiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "float4(" << colour.r << ", " << colour.g << ", " << colour.b << ", " << colour.a << ")";
}

void HLSLShaderCompiler::visit(const TextureNode &node)
{
    *current_stream_ << texture_name(node.texture()) << ".Sample(" << sampler_name(node.texture()->sampler()) << ",";

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
    current_functions_->emplace(blur_function);

    *current_stream_ << "blur(" << texture_name(node.input_node()->texture()) << ",";
        *current_stream_ << " input.tex_coord, " << sampler_name(node.input_node()->texture()->sampler()) << ")";
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
    *current_stream_ << "float4(" << node.value().r << ", " << node.value().g << ", " << node.value().b << ", "
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

void HLSLShaderCompiler::visit(const ColourAdjustNode &node)
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
    float4 bone_weights : TEXCOORD7,
    uint instance_id : SV_InstanceID)
{
    matrix bone_transform = mul(bones[bone_ids[0]], bone_weights[0]);
    bone_transform += mul(bones[bone_ids[1]], bone_weights[1]);
    bone_transform += mul(bones[bone_ids[2]], bone_weights[2]);
    bone_transform += mul(bones[bone_ids[3]], bone_weights[3]);

    PSInput result;

    result.frag_position = mul(position, bone_transform);
    result.frag_position = mul(result.frag_position, model_data[instance_id].model);
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

    *current_stream_ << "float gamma = " << 1.0f / node.description().gamma << ";\n";

    *current_stream_ << R"(
        float3 mapped = fragment_colour.rgb / (fragment_colour.rgb + float3(1.0, 1.0, 1.0));
        mapped = pow(fragment_colour.rgb, float3(gamma, gamma, gamma));
        return float4(mapped.r, mapped.g, mapped.b, 1.0);
    })";
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

    strm << uniforms << '\n';
    strm << "SamplerState sampler_table[] : register(s0);\n";
    strm << "Texture2D texture_table[] : register(t0, space1);\n";
    strm << "TextureCube cube_map_table[] : register(t0, space2);\n";

    for (const auto &function : fragment_functions_)
    {
        strm << function << '\n';
    }

    strm << ps_input << '\n';
    strm << fragment_stream_.str() << '\n';

    return strm.str();
}

}
