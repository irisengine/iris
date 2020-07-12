#include "graphics/material_factory.h"

#include <memory>

#include "graphics/material.h"

namespace
{

// hard coded shaders

static const std::string vertex_source_mesh { R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
typedef struct
{
    float4 position;
    float4 normal;
    float4 color;
    float4 tex;
} VertexIn;
typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
    float4x4 normal_matrix;
} DefaultUniform;
typedef struct
{
    float4 position;
} LightUniform;
typedef struct {
    float4 position [[position]];
    float4 normal [[normal]];
    float4 color;
    float4 tex;
    float4 pos;
} VertexOut;
vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant DefaultUniform *uniform [[buffer(1)]],
    constant LightUniform *light [[buffer(2)]],
    uint vid [[vertex_id]])
{
    VertexOut out;
    out.position = transpose(uniform->projection) * transpose(uniform->view) * transpose(uniform->model) * vertices[vid].position;
    out.pos = transpose(uniform->model) * vertices[vid].position;
    out.normal = transpose(uniform->normal_matrix) * vertices[vid].normal;
    out.color = vertices[vid].color;
    out.tex = vertices[vid].tex;
    return out;
}
)" };

static const std::string fragment_source_mesh { R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
typedef struct {
    float4 position [[position]];
    float4 normal [[normal]];
    float4 color;
    float4 tex;
    float4 pos;
} VertexOut;
typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
    float4x4 normal_matrix;
} DefaultUniform;
typedef struct
{
    float4 position;
} LightUniform;
fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    constant LightUniform *light [[buffer(0)]],
    texture2d<float> main_texture [[texture(0)]]
    )
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);
    float4 sampled_colour = main_texture.sample(s, in.tex.xy);
    float4 amb(0.2f, 0.2f, 0.2f, 1.0f);
    float4 light_colour(1.0f, 1.0f, 1.0f, 1.0f);
    float3 n3 = normalize(in.normal.xyz);
    float4 n = float4(n3.x, n3.y, n3.z, 1);
    float4 light_dir = normalize(light->position - in.pos);
    float diff = max(dot(n, light_dir), 0.0);
    float4 diffuse = light_colour * diff;
    float4 l = amb + diffuse;
    return l * in.color * sampled_colour;
}
)" };


static const std::string vertex_source_sprite { R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float4 position;
    float4 normal;
    float4 color;
    float4 tex;
} VertexIn;

typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
} DefaultUniform;

typedef struct {
    float4 position [[position]];
    float4 color;
    float4 tex;
} VertexOut;

vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant DefaultUniform *uniform [[buffer(1)]],
    uint vid [[vertex_id]])
{
    VertexOut out;

    out.position = transpose(uniform->projection) * transpose(uniform->view) * transpose(uniform->model) * vertices[vid].position;
    out.color = vertices[vid].color;
    out.tex = vertices[vid].tex;
    return out;
}
)" };

static const std::string fragment_source_sprite { R"(

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct {
    float4 position [[position]];
    float4 color;
    float4 tex;
} VertexOut;

typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
} DefaultUniform;

fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    texture2d<float> main_texture [[texture(0)]]
    )
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);
    float4 sampled_colour = main_texture.sample(s, in.tex.xy);

    if(sampled_colour.a < 0.01)
    {
        discard_fragment();
    }

    return in.color * sampled_colour;
}
)" };

}

namespace eng::material_factory
{

Material* sprite()
{
    static auto mat = std::make_unique<Material>(vertex_source_sprite, fragment_source_sprite);
    return mat.get();
}

Material* mesh()
{
    static auto mat = std::make_unique<Material>(vertex_source_mesh, fragment_source_mesh);
    return mat.get();
}

}

