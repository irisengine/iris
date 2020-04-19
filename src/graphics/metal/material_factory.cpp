#include "graphics/material_factory.h"

#include <memory>

#include "graphics/material.h"

namespace
{

// hard coded shaders

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
    device VertexIn *vertices [[Buffer(0)]],
    constant DefaultUniform *uniform [[Buffer(1)]],
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

}

