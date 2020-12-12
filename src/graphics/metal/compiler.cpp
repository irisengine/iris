#include "graphics/render_graph/compiler.h"

#include <sstream>
#include <string>

#include "core/exception.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/blur_node.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/combine_node.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/composite_node.h"
#include "graphics/render_graph/invert_node.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/sin_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_position_node.h"
#include "graphics/texture.h"
#include "graphics/vertex_attributes.h"

namespace
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
typedef struct {
    float4 position [[position]];
    float4 normal [[normal]];
    float4 color;
    float4 tex;
    float4 pos;
    float3 tangent_light_pos;
    float3 tangent_frag_pos;
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
    float time;
} DefaultUniform;
)";

static constexpr auto light_uniform = R"(
typedef struct
{
    float4 position;
} LightUniform;
)";

std::string texture_name(
    iris::Texture *texture,
    std::vector<iris::Texture *> &textures)
{
    const auto find =
        std::find(std::cbegin(textures), std::cend(textures), texture);

    if (find == std::cend(textures))
    {
        textures.emplace_back(texture);
    }

    return "tex" + std::to_string(texture->texture_id());
}

}

namespace iris
{

Compiler::Compiler(const RenderGraph &render_graph)
    : vertex_stream_()
    , fragment_stream_()
    , current_stream_(nullptr)
    , vertex_functions_()
    , fragment_functions_()
    , current_functions_(nullptr)
    , textures_()
{
    render_graph.render_node()->accept(*this);
}

void Compiler::visit(const RenderNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    vertex_stream_ << R"(
    float4x4 bone_transform = uniform->bones[vertices[vid].bone_ids.x] * vertices[vid].bone_weights.x;
    bone_transform += uniform->bones[vertices[vid].bone_ids.y] * vertices[vid].bone_weights.y;
    bone_transform += uniform->bones[vertices[vid].bone_ids.z] * vertices[vid].bone_weights.z;
    bone_transform += uniform->bones[vertices[vid].bone_ids.w] * vertices[vid].bone_weights.w;

    float2 uv = vertices[vid].tex.xy;
)";

    vertex_stream_ << "   float4 position = ";
    auto position_input = node.position_input();
    if (position_input == nullptr)
    {
        vertex_stream_ << "vertices[vid].position;\n";
    }
    else
    {
        position_input->accept(*this);
        vertex_stream_ << ";\n";
    }

    vertex_stream_ << R"(
    VertexOut out;
    out.pos = uniform->model * bone_transform * position;
    out.position = uniform->projection * uniform->view * out.pos;
    out.normal = uniform->normal_matrix * bone_transform * vertices[vid].normal;
    out.color = vertices[vid].color;
    out.tex = vertices[vid].tex;

    float3 T = normalize(float3(uniform->normal_matrix * bone_transform * vertices[vid].tangent));
    float3 B = normalize(float3(uniform->normal_matrix * bone_transform * vertices[vid].bitangent));
    float3 N = normalize(float3(uniform->normal_matrix * bone_transform * vertices[vid].normal));
    float3x3 tbn = transpose(float3x3(T, B, N));

    out.tangent_light_pos = tbn * float3(light->position);
    out.tangent_frag_pos = tbn * float3(out.pos);

    return out;
}
)";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    fragment_stream_ << "    float2 uv = in.tex.xy;";
    fragment_stream_ << "    float4 fragment_colour = ";

    auto colour_input = node.colour_input();
    if (colour_input == nullptr)
    {
        fragment_stream_ << "in.color;";
    }
    else
    {
        colour_input->accept(*this);
        fragment_stream_ << ";\n";
    }

    fragment_stream_ << "    float4 n4 = ";

    auto normal_input = node.normal_input();
    if (normal_input == nullptr)
    {
        fragment_stream_ << "in.normal;";
        fragment_stream_ << "float3 light_dir = normalize(light->position.xyz "
                            "- in.pos.xyz);\n";
    }
    else
    {
        normal_input->accept(*this);
        fragment_stream_ << ";\n";
        fragment_stream_ << "n4 = normalize(n4 * 2.0 - 1.0);\n";
        fragment_stream_
            << "float3 light_dir = normalize(in.tangent_light_pos - "
               "in.tangent_frag_pos);\n";
    }

    fragment_stream_ << R"(

    float3 n = normalize(float3(n4));
    float3 amb = 0.5 * fragment_colour.rgb;
    float diff = max(dot(n, light_dir), 0.0);
    float3 diffuse = diff * fragment_colour.rgb;
    
    return float4(amb + diffuse, 1.0);
}
)";
}

void Compiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "float4(" << colour.x << ", " << colour.y << ", "
                     << colour.z << ", 1.0)";
}

void Compiler::visit(const TextureNode &node)
{
    current_functions_->emplace(R"(
float4 sample_texture(texture2d<float> texture, float2 coord)
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);
    return texture.sample(s, coord);
}
)");

    *current_stream_ << "sample_texture("
                     << texture_name(node.texture(), textures_);

    if (node.texture()->flip())
    {
        *current_stream_ << ", float2(uv.x, -uv.y))";
    }
    else
    {
        *current_stream_ << ", uv)";
    }
}

void Compiler::visit(const InvertNode &node)
{
    current_functions_->emplace(R"(
float4 invert(float4 colour)
{
    return float4(float3(1.0 - colour), 1.0);
})");

    *current_stream_ << "invert(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void Compiler::visit(const BlurNode &node)
{
    current_functions_->emplace(R"(
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
})");

    *current_stream_ << "blur("
                     << texture_name(node.input_node()->texture(), textures_);

    if (node.input_node()->texture()->flip())
    {
        *current_stream_ << ", float2(uv.x, -uv.y))";
    }
    else
    {
        *current_stream_ << ", uv)";
    }
}

void Compiler::visit(const CompositeNode &node)
{
    current_functions_->emplace(R"(
float4 composite(float4 colour1, float4 colour2, float4 depth1, float4 depth2, float2 tex_coord)
{
    float4 colour = colour2;

    if(depth1.r < depth2.r)
    {
        colour = colour1;
    }

    return colour;
}
)");

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

void Compiler::visit(const VertexPositionNode &node)
{
    *current_stream_ << "vertices[vid].position";
}

void Compiler::visit(const ValueNode<float> &node)
{
    *current_stream_ << std::to_string(node.value());
}

void Compiler::visit(const ArithmeticNode &node)
{
    *current_stream_ << "(";
    node.value1()->accept(*this);
    switch (node.arithmetic_operator())
    {
        case ArithmeticOperator::ADD:
            *current_stream_ << " + ";
            break;
        case ArithmeticOperator::SUBTRACT:
            *current_stream_ << " - ";
            break;
        case ArithmeticOperator::MULTIPLY:
            *current_stream_ << " * ";
            break;
        case ArithmeticOperator::DIVIDE:
            *current_stream_ << " / ";
            break;
    }
    node.value2()->accept(*this);
    *current_stream_ << ")";
}

void Compiler::visit(const ComponentNode &node)
{
    node.input_node()->accept(*this);
    *current_stream_ << "." << node.component();
}

void Compiler::visit(const CombineNode &node)
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

void Compiler::visit(const SinNode &node)
{
    *current_stream_ << "sin(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

std::string Compiler::vertex_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << vertex_in << '\n';
    stream << vertex_out << '\n';
    stream << default_uniform << '\n';
    stream << light_uniform << '\n';

    for (const auto &function : vertex_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
 vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant DefaultUniform *uniform [[buffer(1)]],
    constant LightUniform *light [[buffer(2)]],
    uint vid [[vertex_id]])";
    for (auto *texture : textures_)
    {
        const auto id = texture->texture_id();

        stream << "    ,texture2d<float> tex" << id << " [[texture(" << id
               << ")]]" << '\n';
    }
    stream << R"(
    )
{
)";

    stream << vertex_stream_.str() << '\n';

    return stream.str();
}

std::string Compiler::fragment_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << vertex_in << '\n';
    stream << vertex_out << '\n';
    stream << default_uniform << '\n';
    stream << light_uniform << '\n';

    for (const auto &function : fragment_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    constant LightUniform *light [[buffer(0)]])";

    for (auto *texture : textures_)
    {
        const auto id = texture->texture_id();

        stream << "    ,texture2d<float> tex" << id << " [[texture(" << id
               << ")]]" << '\n';
    }
    stream << R"(
    )
{
)";

    stream << fragment_stream_.str() << '\n';

    return stream.str();
}

std::vector<Texture *> Compiler::textures() const
{
    return textures_;
}
}
