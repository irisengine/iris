#include "graphics/render_graph/compiler.h"

#include <sstream>
#include <string>

#include "core/exception.h"
#include "graphics/light.h"
#include "graphics/metal/compiler_strings.h"
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
#include "graphics/render_graph/utils.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_position_node.h"
#include "graphics/texture.h"
#include "graphics/vertex_attributes.h"

namespace
{

/**
 * Helper function to create a variable name for a texture. Always returns the
 * same name for the same texture.
 *
 * @param texture
 *   Texture to generate name for.
 *
 * @param textures
 *   Collection of textures, texture will be inserted if it does not exist.
 *
 * @returns
 *   Unique variable name for the texture.
 */
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

Compiler::Compiler(
    const RenderGraph &render_graph,
    const std::vector<Light *> &lights)
    : vertex_stream_()
    , fragment_stream_()
    , current_stream_(nullptr)
    , vertex_functions_()
    , fragment_functions_()
    , current_functions_(nullptr)
    , textures_()
    , lights_(lights)
{
    render_graph.render_node()->accept(*this);
}

void Compiler::visit(const RenderNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    vertex_stream_ << R"(
    float4x4 bone_transform = calculate_bone_transform(uniform, vid, vertices);

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

    const float3x3 tbn = calculate_tbn(uniform, bone_transform, vid, vertices);

)";

    for (auto i = 0u; i < lights_.size(); ++i)
    {
        vertex_stream_ << replace_index(
            R"(
            out.frag_pos_light_space{} = light[{}].proj * light[{}].view * out.pos;
            out.tangent_light_pos{} = tbn * float3(light[{}].position);
            )",
            i);
    }

    vertex_stream_ << R"(
    out.tangent_frag_pos = tbn * float3(out.pos);

    return out;
}
)";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    if (!node.is_depth_only())
    {
        fragment_stream_ << "float2 uv = in.tex.xy;\n";
        fragment_stream_ << "float4 fragment_colour = ";

        auto colour_input = node.colour_input();
        if (colour_input == nullptr)
        {
            fragment_stream_ << "in.color;\n";
        }
        else
        {
            colour_input->accept(*this);
            fragment_stream_ << ";\n";
        }

        fragment_stream_ << "float4 n4 = ";

        auto normal_input = node.normal_input();
        if (normal_input == nullptr)
        {
            fragment_stream_ << "in.normal;\n";

            for (auto i = 0u; i < lights_.size(); ++i)
            {
                fragment_stream_ << replace_index(
                    R"(
                    float3 light_dir{} = normalize(-(light[{}].position.xyz));
                    )",
                    i);
            }
        }
        else
        {
            normal_input->accept(*this);
            fragment_stream_ << ";\n";
            fragment_stream_ << "n4 = normalize(n4 * 2.0 - 1.0);\n";
            for (auto i = 0u; i < lights_.size(); ++i)
            {
                fragment_stream_ << replace_index(
                    R"(
                    float3 light_dir{} = normalize(-in.tangent_light_pos{});
                    )",
                    i);
            }
        }

        if (lights_.empty())
        {
            fragment_stream_ << "float3 amb = float3(1.0);\n";
        }
        else
        {
            fragment_stream_ << "float3 amb = float3(0.15);\n";
        }

        fragment_stream_ << R"(
    float3 n = normalize(float3(n4));
    float3 diffuse = 0.0;
)";

        for (auto i = 0u; i < lights_.size(); ++i)
        {
            fragment_stream_ << "float shadow" << i << " = 0.0;\n";
            if (node.shadow_map_input(i) != nullptr)
            {
                fragment_stream_
                    << replace_index(
                           R"(
                           shadow{} = calculate_shadow(n, in.frag_pos_light_space{}, light_dir{}, )",
                           i)
                    << texture_name(
                           static_cast<TextureNode *>(node.shadow_map_input(i))
                               ->texture(),
                           textures_)
                    << ");\n";
            }

            fragment_stream_ << replace_index(
                R"(
                float diff{} = max(dot(n, light_dir{}), 0.0);
                float3 diffuse{} = (1.0  - shadow{}) * float3(diff{});
                diffuse += diffuse{};
                )",
                i);
        }

        fragment_stream_ << R"(
    if (fragment_colour.a < 0.01)
    {
        discard_fragment();
    }

    return float4((amb + diffuse) * fragment_colour.rgb, 1.0);
}
)";
    }
    else
    {
        fragment_stream_ << "return float4(0); }\n";
    }
}

void Compiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "float4(" << colour.r << ", " << colour.g << ", "
                     << colour.b << ", " << colour.a << ")";
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
    current_functions_->emplace(invert_function);

    *current_stream_ << "invert(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void Compiler::visit(const BlurNode &node)
{
    current_functions_->emplace(blur_function);

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
    current_functions_->emplace(composite_function);

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
        case ArithmeticOperator::ADD: *current_stream_ << " + "; break;
        case ArithmeticOperator::SUBTRACT: *current_stream_ << " - "; break;
        case ArithmeticOperator::MULTIPLY: *current_stream_ << " * "; break;
        case ArithmeticOperator::DIVIDE: *current_stream_ << " / "; break;
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
    stream << R"(
typedef struct {
    float4 position [[position]];
    float4 normal;
    float4 color;
    float4 tex;
    float4 pos;
    )";
    for (auto i = 0u; i < lights_.size(); ++i)
    {
        stream << "float3 tangent_light_pos" << i << ";\n";
        stream << "float4 frag_pos_light_space" << i << ";\n";
    }
    stream << R"(
    float3 tangent_frag_pos;
} VertexOut;
)";
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
    stream << R"(
typedef struct {
    float4 position [[position]];
    float4 normal;
    float4 color;
    float4 tex;
    float4 pos;
    )";
    for (auto i = 0u; i < lights_.size(); ++i)
    {
        stream << "float3 tangent_light_pos" << i << ";\n";
        stream << "float4 frag_pos_light_space" << i << ";\n";
    }
    stream << R"(
    float3 tangent_frag_pos;
} VertexOut;
)";
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
