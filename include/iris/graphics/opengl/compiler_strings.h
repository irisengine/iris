////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sstream>
#include <string>

namespace iris
{

static constexpr auto preamble = R"(
#version 430 core
precision mediump float;

#extension GL_ARB_bindless_texture : require
)";

static constexpr auto layouts = R"(
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec4 tex;
layout (location = 4) in vec4 tangent;
layout (location = 5) in vec4 bitangent;
layout (location = 6) in ivec4 bone_ids;
layout (location = 7) in vec4 bone_weights;

invariant gl_Position;
)";

static constexpr auto camera_data_block = R"(
layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
    vec3 camera;
};
)";

static constexpr auto bone_data_block = R"(
layout (std140, binding = 1) uniform BoneData
{
    mat4 bones[100];
};
)";

static constexpr auto light_data_block = R"(
layout (std140, binding = 2) uniform LightData
{
    mat4 light_projection;
    mat4 light_view;
    vec4 light_colour;
    vec4 light_position;
    vec4 light_attenuation;
};
)";

static constexpr auto texture_table_block = R"(
layout (std430, binding = 3) buffer TextureData
{
    uvec2 texture_table[];
};
)";

static constexpr auto cube_map_table_block = R"(
layout (std430, binding = 4) buffer CubeMapData
{
    uvec2 cube_map_table[];
};
)";

static constexpr auto model_data_block = R"(

struct Model
{
    mat4 model;
    mat4 normal_matrix;
};

layout (std430, binding = 5) buffer ModelData
{
    Model models[];
};
)";

static constexpr auto vertex_out = R"(
out vec4 vertex_pos;
out vec4 frag_pos;
out vec2 tex_coord;
out vec4 col;
out vec4 norm;
out vec4 frag_pos_light_space;
out vec3 tangent_light_pos;
out vec3 tangent_view_pos;
out vec3 tangent_frag_pos;
)";

static constexpr auto fragment_in = R"(
in vec4 vertex_pos;
in vec2 tex_coord;
in vec4 col;
in vec4 norm;
in vec4 frag_pos;
in vec4 frag_pos_light_space;
in vec3 tangent_light_pos;
in vec3 tangent_view_pos;
in vec3 tangent_frag_pos;
)";

static constexpr auto fragment_out = R"(
out vec4 outColour;
)";

static constexpr auto vertex_begin = R"(
    mat4 bone_transform = calculate_bone_transform(bone_ids, bone_weights);
    mat3 tbn = calculate_tbn(bone_transform, transpose(models[gl_InstanceID].normal_matrix));

    vertex_pos = position;
    frag_pos = transpose(models[gl_InstanceID].model) * bone_transform * position;
    gl_Position = transpose(projection) * transpose(view) * frag_pos;
    norm = transpose(models[gl_InstanceID].normal_matrix) * bone_transform * normal;
    col = colour;
    tex_coord = vec2(tex.x, tex.y);
)";

static constexpr auto blur_function = R"(
vec4 blur(sampler2D tex, vec2 tex_coords)
{
    const float offset = 1.0 / 500.0;  
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(tex, tex_coords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * kernel[i];
    }
    return vec4(col, 1.0);
})";

static constexpr auto composite_function = R"(
vec4 composite(vec4 colour1, vec4 colour2, vec4 depth1, vec4 depth2, vec2 tex_coord)
{
    vec4 colour = colour2;

    if(depth1.r < depth2.r)
    {
        colour = colour1;
    }

    return colour;
}
)";

static constexpr auto invert_function = R"(
vec4 invert(vec4 colour)
{
    return vec4(vec3(1.0 - colour), 1.0);
})";

static constexpr auto bone_transform_function = R"(
mat4 calculate_bone_transform(ivec4 bone_ids, vec4 bone_weights)
{
    mat4 bone_transform = bones[bone_ids[0]] * bone_weights[0];
    bone_transform += bones[bone_ids[1]] * bone_weights[1];
    bone_transform += bones[bone_ids[2]] * bone_weights[2];
    bone_transform += bones[bone_ids[3]] * bone_weights[3];

    return transpose(bone_transform);

})";

static constexpr auto tbn_function = R"(
mat3 calculate_tbn(mat4 bone_transform, mat4 normal_matrix)
{
    vec3 T = normalize(vec3(normal_matrix * bone_transform * tangent));
    vec3 B = normalize(vec3(normal_matrix * bone_transform * bitangent));
    vec3 N = normalize(vec3(normal_matrix * bone_transform * normal));

    return transpose(mat3(T, B, N));
})";

static constexpr auto shadow_function = R"(
float calculate_shadow(vec3 n, vec4 frag_pos_light_space, vec3 light_dir, sampler2D tex)
{
    float shadow = 0.0;

    vec3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coord = proj_coord * 0.5 + 0.5;

    float closest_depth = texture(tex, proj_coord.xy).r;
    float current_depth = proj_coord.z;

    float bias = 0.001;
    shadow = (current_depth - bias) > closest_depth ? 1.0 : 0.0;

    if(proj_coord.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
})";

}
