#pragma once

#include <sstream>
#include <string>

namespace iris
{

static constexpr auto preamble = R"(
#version 330 core
precision mediump float;
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
)";

static constexpr auto uniforms = R"(
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal_matrix;
uniform mat4 bones[100];
uniform vec3 camera;
)";

static constexpr auto vertex_out = R"(
out vec4 frag_pos;
out vec2 tex_coord;
out vec4 col;
out vec4 norm;
out vec3 tangent_view_pos;
out vec3 tangent_frag_pos;
)";

static constexpr auto fragment_in = R"(
in vec2 tex_coord;
in vec4 col;
in vec4 norm;
in vec4 frag_pos;
in vec3 tangent_view_pos;
in vec3 tangent_frag_pos;
)";

static constexpr auto fragment_out = R"(
out vec4 outColor;
)";

static constexpr auto vertex_begin = R"(
    mat4 bone_transform = calculate_bone_transform(bone_ids, bone_weights);
    mat3 tbn = calculate_tbn(bone_transform);

    col = colour;
    norm = normal_matrix * bone_transform * normal;
    tex_coord = vec2(tex.x, tex.y);
    frag_pos = model * bone_transform * position;
    gl_Position = projection * view * frag_pos;

)";

static constexpr auto blur_function = R"(
vec4 blur(sampler2D tex, vec2 tex_coords)
{
    const float offset = 1.0 / 100.0;  
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

    return bone_transform;

})";

static constexpr auto tbn_function = R"(
mat3 calculate_tbn(mat4 bone_transform)
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

    float bias = max(0.05 * (1.0 - dot(n, light_dir)), 0.005);
    shadow = (current_depth - bias) > closest_depth ? 1.0 : 0.0;

    if(proj_coord.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
})";

}
