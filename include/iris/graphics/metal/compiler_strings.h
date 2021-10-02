#pragma once

#include <string>

namespace iris
{

static constexpr auto preamble = R"(
#include <metal_relational>
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
)";

static constexpr auto vertex_in = R"(
typedef struct
{
    float4 position;
    float4 normal;
    float4 color;
    float4 tex;
    float4 tangent;
    float4 bitangent;
    int4 bone_ids;
    float4 bone_weights;
} VertexIn;
)";

static constexpr auto vertex_out = R"(
typedef struct
{
    float4 position [[position]];
    float4 frag_position;
    float3 tangent_view_pos;
    float3 tangent_frag_pos;
    float3 tangent_light_pos;
    float4 frag_pos_light_space;
    float4 normal;
    float4 color;
    float4 tex;
} VertexOut;
)";

static constexpr auto default_uniform = R"(
typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
    float4x4 normal_matrix;
    float4x4 bones[100];
    float4 camera;
    float4 light_colour;
    float4 light_position;
    float light_attenuation[3];
    float time;
} DefaultUniform;
)";

static constexpr auto directional_light_uniform = R"(
typedef struct
{
    float4x4 proj;
    float4x4 view;
} DirectionalLightUniform;
)";

static constexpr auto point_light_uniform = R"(
typedef struct
{
    float4 position;
} PointLightUniform;
)";

static constexpr auto vertex_begin = R"(
float4x4 bone_transform = calculate_bone_transform(uniform, vid, vertices);
float2 uv = vertices[vid].tex.xy;

VertexOut out;
out.frag_position = uniform->model * bone_transform * vertices[vid].position;
out.position = uniform->projection * uniform->view * out.frag_position;
out.normal = uniform->normal_matrix * bone_transform * vertices[vid].normal;
out.color = vertices[vid].color;
out.tex = vertices[vid].tex;

const float3x3 tbn = calculate_tbn(uniform, bone_transform, vid, vertices);

out.tangent_light_pos = tbn * uniform->light_position.xyz;
out.tangent_view_pos = tbn * uniform->camera.xyz;
out.tangent_frag_pos = tbn * out.frag_position.xyz;
)";

static constexpr auto blur_function = R"(
float4 blur(texture2d<float> texture, float2 tex_coords)
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);

    const float offset = 1.0 / 100.0;  
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
        sampleTex[i] = float3(texture.sample(s, tex_coords + offsets[i]));
    }
    float3 col = float3(0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * k[i];
    }
    return float4(col, 1.0);
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

static constexpr auto invert_function = R"(
float4 invert(float4 colour)
{
    return float4(float3(1.0 - colour), 1.0);
})";

static constexpr auto bone_transform_function = R"(
float4x4 calculate_bone_transform(constant DefaultUniform *uniform, uint vid, device VertexIn *vertices)
{
    float4x4 bone_transform = uniform->bones[vertices[vid].bone_ids.x] * vertices[vid].bone_weights.x;
    bone_transform += uniform->bones[vertices[vid].bone_ids.y] * vertices[vid].bone_weights.y;
    bone_transform += uniform->bones[vertices[vid].bone_ids.z] * vertices[vid].bone_weights.z;
    bone_transform += uniform->bones[vertices[vid].bone_ids.w] * vertices[vid].bone_weights.w;

    return transpose(bone_transform);

})";

static constexpr auto tbn_function = R"(
float3x3 calculate_tbn(constant DefaultUniform *uniform, float4x4 bone_transform, uint vid, device VertexIn *vertices)
{
    float3 T = normalize(float3(uniform->normal_matrix * bone_transform * vertices[vid].tangent));
    float3 B = normalize(float3(uniform->normal_matrix * bone_transform * vertices[vid].bitangent));
    float3 N = normalize(float3(uniform->normal_matrix * bone_transform * vertices[vid].normal));
    return transpose(float3x3(T, B, N));
})";

static constexpr auto shadow_function = R"(
float calculate_shadow(float3 n, float4 frag_pos_light_space, float3 light_dir, texture2d<float> texture, sampler smp)
{
    float shadow = 0.0;

    float3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;

    float2 proj_uv = float2(proj_coord.x, -proj_coord.y);
    proj_uv = proj_uv * 0.5 + 0.5;

    float closest_depth = texture.sample(smp, proj_uv).r;
    float current_depth = proj_coord.z;
    float bias = 0.001;

    shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
    if(proj_coord.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
})";

inline std::string replace_index(const std::string &input, std::size_t index)
{
    std::stringstream strm;

    static const std::string delimiter = "{}";
    auto start = 0u;
    auto end = input.find(delimiter);

    while (end != std::string::npos)
    {
        strm << input.substr(start, end - start) << index;
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }

    strm << input.substr(start, end);

    return strm.str();
}

}
