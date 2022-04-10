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
#include "graphics/render_graph/post_processing/gamma_correct_node.h"
#include "graphics/render_graph/post_processing/tone_map_node.h"
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
    matrix inverse_projection;
    matrix inverse_view;
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
    precise float2 tex_coord : TEXCOORD;
    precise float4 vertex_position : TEXCOORD1;
    precise float4 frag_position : TEXCOORD2;
    precise float4 view_position : COLOR1;
    precise float4 tangent_view_pos : TEXCOORD4;
    precise float4 tangent_frag_pos : TEXCOORD5;
    precise float4 tangent_light_pos : TEXCOORD6;
    precise float4 frag_pos_light_space : TEXCOORD7;
    precise float4 normal : NORMAL;
    precise float4 view_normal : NORMAL1;
    precise float4 colour : COLOR;
    precise float4 tmp : COLOR2;
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
    float offset[5] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float weight[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

    float4 colour = tex.Sample(smpler, tex_coords) * weight[0];

    for (int i = 1; i < 5; ++i)
    {
        colour += tex.Sample(smpler, tex_coords + float2(0.0f, offset[i] / 800.0f)) * weight[i];
        colour += tex.Sample(smpler, tex_coords - float2(0.0f, offset[i] / 800.0f)) * weight[i];
    }

    for (int i = 1; i < 5; ++i)
    {
        colour += tex.Sample(smpler, tex_coords + float2(offset[i] / 800.0f, 0.0f)) * weight[i];
        colour += tex.Sample(smpler, tex_coords - float2(offset[i] / 800.0f, 0.0f)) * weight[i];
    }

    return colour;
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
HLSLShaderCompiler::HLSLShaderCompiler(
    const RenderGraph *render_graph,
    LightType light_type,
    bool render_to_normal_target,
    bool render_to_position_target)
    : vertex_stream_()
    , fragment_stream_()
    , current_stream_(nullptr)
    , vertex_functions_()
    , fragment_functions_()
    , current_functions_(nullptr)
    , light_type_(light_type)
    , render_to_normal_target_(render_to_normal_target)
    , render_to_position_target_(render_to_position_target)
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

    result.vertex_position = position;
    result.frag_position = mul(result.vertex_position, bone_transform);
    result.frag_position = mul(result.frag_position, model_data[instance_id].model);
    result.view_position = mul(result.frag_position, view);
    result.position = mul(result.view_position, projection);
    result.tmp = result.position;

    result.normal = mul(normal, bone_transform);
    result.normal = mul(result.normal, model_data[instance_id].normal_matrix);
    result.view_normal = mul(result.normal, inverse_view);
    result.colour = colour;
    result.tex_coord = tex_coord;

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

    return result;
})";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    if (!render_to_normal_target_ && !render_to_position_target_)
    {
        *current_stream_ << R"(
float4 main(PSInput input) : SV_TARGET
{)";
    }
    else
    {
        *current_stream_ << R"(
        struct PS_OUTPUT
        {
            float4 colour: SV_Target0;
            )";

        if (render_to_normal_target_)
        {
            *current_stream_ << "float4 normal: SV_Target1;\n";
        }

        if (render_to_position_target_)
        {
            *current_stream_ << "float4 position: SV_Target2;\n";
        }

        *current_stream_ << R"(
        };

PS_OUTPUT main(PSInput input)
{)";
    }

    build_fragment_colour(*current_stream_, node.colour_input(), this);
    build_normal(*current_stream_, node.normal_input(), this);

    *current_stream_ << "float ambient_occlusion = ";
    visit_or_default(*current_stream_, node.ambient_occlusion_input(), this, "1.0f");

    *current_stream_ << "float4 out_colour;\n";

    // depending on the light type depends on how we interpret the light
    // constant data and how we calculate lighting
    switch (light_type_)
    {
        case LightType::AMBIENT:
            *current_stream_ << "out_colour = light_colour * fragment_colour * float4(ambient_occlusion, "
                                "ambient_occlusion, ambient_occlusion, 1.0);\n";
            break;
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
                
                out_colour = float4(diffuse * fragment_colour, 1.0);
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
                
                out_colour = float4(diffuse * light_colour.xyz * fragment_colour.xyz * att, 1.0);
                )";
            break;
        default: throw Exception("unknown light type");
    }

    if (!render_to_normal_target_ && !render_to_position_target_)
    {
        *current_stream_ << "return out_colour;\n";
    }
    else
    {
        *current_stream_ << R"(
            PS_OUTPUT output;
            output.colour = out_colour;
            )";

        if (render_to_normal_target_)
        {
            *current_stream_ << "output.normal = float4(normalize(input.view_normal.xyz), 1.0f);\n";
        }

        if (render_to_position_target_)
        {
            *current_stream_ << "output.position = input.view_position;\n";
            //*current_stream_ << "output.position /= output.position.w;\n";
            //*current_stream_ << "output.position.z *= -1;\n";
        }

        *current_stream_ << R"(
                return output;
            )";
    }

    *current_stream_ << "}";
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
    switch (node.uv_mode())
    {
        case UVMode::VERTEX_DATA:
        {
            *current_stream_ << texture_name(node.texture()) << ".Sample(" << sampler_name(node.texture()->sampler())
                             << ", input.tex_coord)";
        }
        break;
        case UVMode::SCREEN_SPACE:
        {
            *current_stream_ << texture_name(node.texture()) << ".Sample(" << sampler_name(node.texture()->sampler())
                             << ", input.position.xy * "
                             << "float2(" << 1.0f / node.texture()->width() << "f, " << 1.0f / node.texture()->height()
                             << "f))\n";
        }
        break;
        default: throw Exception("unknown uv mode");
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

void HLSLShaderCompiler::visit(const AmbientOcclusionNode &node)
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
    PSInput result;

    result.position = position;
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

    const auto *input_texture = static_cast<const TextureNode *>(node.colour_input())->texture();
    const auto *normal_texture = node.normal_texture()->texture();
    const auto *position_texture = node.position_texture()->texture();

    *current_stream_ << "float3 samples[] = {\n";

    for (auto i = 0u; i < 64u; ++i)
    {
        Vector3 sample{random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), random_float(0.0f, 1.0f)};
        sample.normalise();
        sample *= random_float(0.0f, 1.0f);

        const auto scale = static_cast<float>(i) / 64.0f;
        sample *= Vector3::lerp(Vector3(0.1f), Vector3(1.0f), scale * scale);
        // sample = Vector3::lerp(sample * 0.1f, sample, scale * scale);

        *current_stream_ << "float3(" << sample.x << ", " << sample.y << ", " << sample.z << "), ";
    }

    *current_stream_ << "};\n";

    *current_stream_ << "float3 noise_data[] = {\n";

    for (auto i = 0u; i < 16u; ++i)
    {
        *current_stream_ << "float3(" << std::to_string(random_float(-1.0f, 1.0f)) << ", "
                         << std::to_string(random_float(-1.0f, 1.0f)) << ", "
                         << "0.0f), ";
    }

    *current_stream_ << "};\n";
    *current_stream_ << "float2 size = float2(" << input_texture->width() << ".0f, " << input_texture->height()
                     << ".0f);\n";

    *current_stream_ << "float2 uv = input.tex_coord.xy;\n";
    *current_stream_ << "float3 frag_pos = " << texture_name(position_texture) << ".Sample("
                     << sampler_name(position_texture->sampler()) << ", uv).xyz;\n";
    *current_stream_ << "float3 normal = normalize(" << texture_name(normal_texture) << ".Sample("
                     << sampler_name(normal_texture->sampler()) << ", uv).rgb);\n";
    *current_stream_ << R"(
        int x = int(uv.x * size.x) % 4;
        int y = int(uv.y * size.y) % 4;
        int index = (y * 4) + x;
        float3 rand = normalize(noise_data[index]);

        float3 tangent = normalize(rand - normal * dot(rand, normal));
        float3 bitangent = cross(normal, tangent);
        float3x3 tbn = float3x3(tangent, bitangent, normal);

        int kernelSize = 64;
        float radius = 0.5;
        float bias = 0.025;
        float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        float3 samplePos = mul(float4(samples[i], 1.0), tbn); // from tangent to view-space
        samplePos = frag_pos + samplePos * radius; 

        float4 offset = mul(samplePos , projection);
        offset.xy /= offset.w;
        offset.x = offset.x * 0.5f + 0.5f;
        offset.y = -offset.y * 0.5f + 0.5f;

      )";
    *current_stream_ << "float sampleDepth = " << texture_name(position_texture) << ".Sample("
                     << sampler_name(position_texture->sampler()) << ", offset.xy).z;\n";
    *current_stream_ << R"(
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    
        //return float4(frag_pos.zzz, 1.0);
        //return float4(frag_pos, 1);
        return light_colour * fragment_colour * float4(occlusion, occlusion, occlusion, 1.0f);
    })";
}

void HLSLShaderCompiler::visit(const ToneMapNode &node)
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

    *current_stream_ << R"(
        float3 mapped = fragment_colour.rgb / (fragment_colour.rgb + float3(1.0, 1.0, 1.0));
        return float4(mapped.r, mapped.g, mapped.b, 1.0);
    })";
}

void HLSLShaderCompiler::visit(const GammaCorrectNode &node)
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

    *current_stream_ << R"(
        float3 mapped = pow(fragment_colour.rgb, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
        return float4(mapped.r, mapped.g, mapped.b, 1.0);
    })";
}

void HLSLShaderCompiler::visit(const AntiAliasingNode &node)
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
    current_functions_->emplace(R"(
        float rgb_to_luma(float3 rgb)
        {
            return rgb.y * (0.587/0.299) + rgb.x;
            //return sqrt(dot(rgb, float3(0.299f, 0.587f, 0.114f)));
        }
        )");

    // build fragment shader

    *current_stream_ << R"(
float4 main(PSInput input) : SV_TARGET
{)";

    build_fragment_colour(*current_stream_, node.colour_input(), this);

    const auto *tex_node = static_cast<const TextureNode *>(node.colour_input());

    current_functions_->emplace(R"(
            float2 adjust_uv(float2 uv)
            {
                return float2(uv.x, uv.y);
            }
        )");

    *current_stream_ << R"(
            int up_offset = -1;
            int down_offset = 1;
            int left_offset = -1;
            int right_offset = 1;
        )";

    *current_stream_ << "float2 uv = input.tex_coord.xy;\n";
    *current_stream_ << "Texture2D tex = " << texture_name(tex_node->texture()) << ";\n";
    *current_stream_ << "SamplerState smplr = " << sampler_name(tex_node->texture()->sampler()) << ";\n";
    *current_stream_ << "float2 inverse_size = float2(" << 1.0f / tex_node->texture()->width() << "f, "
                     << 1.0f / tex_node->texture()->height() << "f);\n";

    *current_stream_ << R"(

        //return float4(Fxaa(uv, tex, smplr, float2(1.0f / 800.0f, 1.0f / 800.0f)), 1);

        float3 colour_centre = tex.Sample(smplr, uv).rgb;
        float luma_centre = rgb_to_luma(colour_centre);
        
        float luma_down = rgb_to_luma(tex.Sample(smplr, uv, int2(0, down_offset)).rgb);
        float luma_up = rgb_to_luma(tex.Sample(smplr, uv, int2(0, up_offset)).rgb);
        float luma_left = rgb_to_luma(tex.Sample(smplr, uv, int2(left_offset, 0)).rgb);
        float luma_right = rgb_to_luma(tex.Sample(smplr, uv, int2(right_offset, 0)).rgb);
        
        float luma_min = min(luma_centre, min(min(luma_down, luma_up), min(luma_left, luma_right)));
        float luma_max = max(luma_centre, max(max(luma_down, luma_up), max(luma_left, luma_right)));
        
        float luma_range = luma_max - luma_min;
        
        if (luma_range < max(0.0312f, 0.125f * luma_max))
        {
            return float4(colour_centre, 1.0);
        }
        
        float luma_down_left = rgb_to_luma(tex.Sample(smplr, uv, int2(left_offset, down_offset)).rgb);
        float luma_up_right = rgb_to_luma(tex.Sample(smplr, uv, int2(right_offset, up_offset)).rgb);
        float luma_up_left = rgb_to_luma(tex.Sample(smplr, uv, int2(left_offset, up_offset)).rgb);
        float luma_down_right = rgb_to_luma(tex.Sample(smplr, uv, int2(right_offset, down_offset)).rgb);
        
        float luma_down_up = luma_down + luma_up;
        float luma_left_right = luma_left + luma_right;
        
        float luma_left_corners = luma_down_left + luma_up_left;
        float luma_down_corners = luma_down_left + luma_down_right;
        float luma_right_corners = luma_down_right + luma_up_right;
        float luma_up_corners = luma_up_right + luma_up_left;
        
        float edge_h =  abs(-2.0f * luma_left + luma_left_corners) +
                        abs(-2.0f * luma_centre + luma_down_up) * 2.0f +
                        abs(-2.0f * luma_right + luma_right_corners);
        
        float edge_v =  abs(-2.0f * luma_up + luma_up_corners) +
                        abs(-2.0f * luma_centre + luma_left_right) * 2.0f +
                        abs(-2.0f * luma_down + luma_down_corners);
        
        bool is_horiz = (edge_h >= edge_v);

        float luma_neg = is_horiz ? luma_up : luma_left;
        float luma_pos = is_horiz ? luma_down : luma_right;
        
        float gradient_neg = abs(luma_neg - luma_centre);
        float gradient_pos = abs(luma_pos - luma_centre);
        
        float step_length = is_horiz ? -inverse_size.y : -inverse_size.x;
        
        float luma_local_avg = 0.0f;
        float gradient_scaled = 0.0f;
        if (gradient_neg < gradient_pos)
        {
            luma_local_avg = 0.5f * (luma_pos + luma_centre);
            gradient_scaled = gradient_pos;
            step_length = -step_length;
        }
        else
        {
            luma_local_avg = 0.5f * (luma_neg + luma_centre);
            gradient_scaled = gradient_neg;
        }

        float2 current_uv = uv;
        if (is_horiz)
        {
            current_uv.y += step_length * 0.5f;
        }
        else
        {
            current_uv.x += step_length * 0.5f;
        }

        gradient_scaled *= 0.25f;
        
        float2 offset = is_horiz ? float2(inverse_size.x, 0.0f) : float2(0.0f, inverse_size.y);
        
        float2 uv1 = current_uv - offset;
        float2 uv2 = current_uv + offset;

        float luma_end1 = 0.0f;
        float luma_end2 = 0.0f;

        bool reached1 = false;
        bool reached2 = false;

        for (int i = 0; i < 24; ++i)
        {
            float steps[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f,
                              8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f };
            if (!reached1)
            {
                luma_end1 = rgb_to_luma(tex.SampleLevel(smplr, uv1, 0).rgb) - luma_local_avg;
            }

            if (!reached2)
            {
                luma_end2 = rgb_to_luma(tex.SampleLevel(smplr, uv2, 0).rgb) - luma_local_avg;
            }
            
            reached1 = reached1 || abs(luma_end1) >= gradient_scaled;
            reached2 = reached2 || abs(luma_end2) >= gradient_scaled;

            if (reached1 && reached2)
            {
                break;
            }

            if (!reached1)
            {
                uv1 -= offset * steps[0];
            }

            if (!reached2)
            {
                uv2 += offset * steps[0];
            }
        }

        
        float distance1 = is_horiz ? (uv.x - uv1.x) : (uv.y - uv1.y);
        float distance2 = is_horiz ? (uv2.x - uv.x) : (uv2.y - uv.y);
        
        bool is_direction1 = distance1 < distance2;
        float distance_final = min(distance1, distance2);
        
        float edge_thickness = (distance1 + distance2);
        //return float4(distance1, distance2, 0, 1);
        
        float pixel_offset = -distance_final / edge_thickness + 0.5f;
        
        bool is_luma_centre_smaller = luma_centre < luma_local_avg;
        bool correct_variation = ((is_direction1 ? luma_end1 : luma_end2) < 0.0f) != is_luma_centre_smaller;
        float final_offset = correct_variation ? pixel_offset : 0.0f;
        
        float luma_average = (1.0f / 12.0f) * (2.0f * (luma_down_up + luma_left_right) + luma_left_corners + luma_right_corners);
        float sub_pixel_offset1 = clamp(abs(luma_average - luma_centre) / luma_range, 0.0f, 1.0f);
        float sub_pixel_offset2 = (-2.0f * sub_pixel_offset1 + 3.0f) * sub_pixel_offset1 * sub_pixel_offset1;
        float sub_pixel_offset_final = sub_pixel_offset2 * sub_pixel_offset2 * 0.75f;
        
        final_offset = max(final_offset, sub_pixel_offset_final);
        
        
        float2 final_uv = uv;
        if (is_horiz)
        {
            final_uv.y += final_offset * step_length;
        }
        else
        {
            final_uv.x += final_offset * step_length;
        }
        
        return tex.SampleLevel(smplr, float2(final_uv.x, final_uv.y), 0);
    }
    )";

    /*
     */
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
