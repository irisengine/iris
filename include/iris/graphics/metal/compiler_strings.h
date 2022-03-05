////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
struct VertexIn
{
    float4 position;
    float4 normal;
    float4 color;
    float4 tex;
    float4 tangent;
    float4 bitangent;
    int4 bone_ids;
    float4 bone_weights;
};
)";

static constexpr auto vertex_out = R"(
struct VertexOut
{
    float4 position [[position]];
    float4 vertex_position;
    float4 frag_position;
    float3 tangent_view_pos;
    float3 tangent_frag_pos;
    float3 tangent_light_pos;
    float4 frag_pos_light_space;
    float4 normal;
    float4 color;
    float4 tex;
};
)";

static constexpr auto bone_data_struct = R"(
struct BoneData
{
    float4x4 bones[100];
};
)";

static constexpr auto camera_data_struct = R"(
struct CameraData
{
    float4x4 projection;
    float4x4 view;
    float4 camera;
};
)";

static constexpr auto light_data_struct = R"(
struct LightData
{
    float4 colour;
    float4 position;
    float4 attenuation;
    float4x4 proj;
    float4x4 view;
};
)";

static constexpr auto model_data_struct = R"(
struct ModelData
{
    float4x4 model;
    float4x4 normal_matrix;
};
)";

static constexpr auto texture_struct = R"(
struct Texture
{
    texture2d<float> texture;
};
)";

static constexpr auto cube_map_struct = R"(
struct CubeMap
{
    texturecube<float> cube_map;
};
)";

static constexpr auto vertex_begin = R"(
float4x4 bone_transform = calculate_bone_transform(bone_data, vid, vertices);
float2 uv = vertices[vid].tex.xy;

VertexOut out;
out.vertex_position = vertices[vid].position;
out.frag_position = transpose(model_data[instance_id].model) * bone_transform * vertices[vid].position;
out.position = transpose(camera_data->projection) * transpose(camera_data->view) * out.frag_position;
out.normal = transpose(model_data[instance_id].normal_matrix) * bone_transform * vertices[vid].normal;
out.color = vertices[vid].color;
out.tex = vertices[vid].tex;

const float3x3 tbn = calculate_tbn(bone_transform, vid, vertices, transpose(model_data[instance_id].normal_matrix));

out.tangent_light_pos = tbn * light_data->position.xyz;
out.tangent_view_pos = tbn * camera_data->camera.xyz;
out.tangent_frag_pos = tbn * out.frag_position.xyz;
)";

static constexpr auto blur_function = R"(
float4 blur(texture2d<float> texture, float2 tex_coords)
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);

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
float4x4 calculate_bone_transform(constant BoneData *bone_data, uint vid, device VertexIn *vertices)
{
    float4x4 bone_transform = bone_data->bones[vertices[vid].bone_ids.x] * vertices[vid].bone_weights.x;
    bone_transform += bone_data->bones[vertices[vid].bone_ids.y] * vertices[vid].bone_weights.y;
    bone_transform += bone_data->bones[vertices[vid].bone_ids.z] * vertices[vid].bone_weights.z;
    bone_transform += bone_data->bones[vertices[vid].bone_ids.w] * vertices[vid].bone_weights.w;

    return transpose(bone_transform);

})";

static constexpr auto tbn_function = R"(
float3x3 calculate_tbn(float4x4 bone_transform, uint vid, device VertexIn *vertices, float4x4 normal_matrix)
{
    float3 T = normalize(float3(normal_matrix * bone_transform * vertices[vid].tangent));
    float3 B = normalize(float3(normal_matrix * bone_transform * vertices[vid].bitangent));
    float3 N = normalize(float3(normal_matrix * bone_transform * vertices[vid].normal));
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

}
