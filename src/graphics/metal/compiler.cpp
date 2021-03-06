#include "graphics/render_graph/compiler.h"

#include <sstream>
#include <string>

#include "core/exception.h"
#include "graphics/lights/lighting_rig.h"
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
    iris::Compiler *visitor,
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
 * Write shader code for generating tangent values to a stream.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param lighting_rig
 *   Lights used in shader.
 */
void build_tangent_values(
    std::stringstream &strm,
    const iris::LightingRig *lighting_rig)
{
    const auto directional_light_count =
        lighting_rig->directional_lights.size();
    const auto point_light_count = lighting_rig->point_lights.size();

    for (auto i = 0u; i < directional_light_count; ++i)
    {
        strm << iris::replace_index(
            R"(
            out.frag_pos_light_space{} = directional_lights[{}].proj * directional_lights[{}].view * out.pos;
            out.tangent_light_pos{} = tbn * directional_lights[{}].position.xyz;
            )",
            i);
    }

    for (auto i = 0u; i < point_light_count; ++i)
    {
        strm << iris::replace_index(
            R"(
            out.tangent_light_pos{} = tbn * point_light[<>].position.xyz;
            )",
            i + directional_light_count,
            i);
    }

    strm << R"(
        out.tangent_view_pos = tbn * uniform->camera.xyz;
        out.tangent_frag_pos = tbn * out.pos.xyz;
        return out;
    })";
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
void build_fragment_colour(
    std::stringstream &strm,
    const iris::Node *colour,
    iris::Compiler *visitor)
{
    strm << "float4 fragment_colour = ";
    visit_or_default(strm, colour, visitor, "in.color");
}

/**
 * Write shader code for generating specular values.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param specular_power
 *   Node for specular power (maybe nullptr).
 *
 * @param specular_amount
 *   Node for specular amount (maybe nullptr).
 *
 * @param visitor
 *   Visitor for node.
 */
void build_specular_values(
    std::stringstream &strm,
    const iris::Node *specular_power,
    const iris::Node *specular_amount,
    iris::Compiler *visitor)
{
    strm << "float specular_power =";
    visit_or_default(strm, specular_power, visitor, "32.0");

    strm << "float specular_amount = ";
    visit_or_default(strm, specular_amount, visitor, "1.0");
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
void build_normal(
    std::stringstream &strm,
    const iris::Node *normal,
    iris::Compiler *visitor)
{
    strm << "float3 n = ";
    if (normal == nullptr)
    {
        strm << "normalize(in.normal.xyz);\n";
    }
    else
    {
        strm << "float3(";
        normal->accept(*visitor);
        strm << ");\n";
        strm << "n = normalize(n * 2.0 - 1.0);\n";
    }
}

/**
 * Write shader code for generating light directional values.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param has_normal_input
 *   True if material has a normal input node, otherwise false.
 *
 * @param lighting_rig
 *   Lights used in shader.
 */
void build_direction_values(
    std::stringstream &strm,
    bool has_normal_input,
    const iris::LightingRig *lighting_rig)
{
    for (auto i = 0u; i < lighting_rig->directional_lights.size(); ++i)
    {
        strm << iris::replace_index(
            (!has_normal_input
                 ? "float3 light_dir{} = "
                   "normalize(-(directional_lights[{}].position.xyz));\n"
                 : "float3 light_dir{} = "
                   "normalize(-in.tangent_light_pos{});\n"),
            i);
    }

    for (auto i = 0u; i < lighting_rig->point_lights.size(); ++i)
    {
        strm << iris::replace_index(
            (!has_normal_input
                 ? "float3 light_dir{} = "
                   "normalize(point_light[<>].position.xyz - in.pos.xyz);\n"
                 : "float3 light_dir{} = normalize(in.tangent_light_pos{} - "
                   "in.tangent_frag_pos.xyz);\n"),
            i + lighting_rig->directional_lights.size(),
            i);
    }

    strm
        << (!has_normal_input
                ? "float3 view_dir = normalize(uniform->camera.xyz - "
                  "in.pos.xyz);\n"
                : "float3 view_dir = normalize(in.tangent_view_pos.xyz - "
                  "in.tangent_frag_pos.xyz);\n");
}

/**
 * Write shader code for generating light calculations.
 *
 * @param strm
 *   Stream to write to.
 *
 * @param node
 *   Root node for render graph.
 *
 * @param lighting_rig
 *   Lights used in shader.
 *
 * @param textures
 *   Collection of textures.
 */
void build_lighting_calculations(
    std::stringstream &strm,
    const iris::RenderNode &node,
    const iris::LightingRig *lighting_rig,
    std::vector<iris::Texture *> &textures)
{
    const auto directional_light_count =
        lighting_rig->directional_lights.size();
    const auto point_light_count = lighting_rig->point_lights.size();

    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        strm << "float shadow" << i << " = 0.0;\n";
        if (i < directional_light_count && node.shadow_map_input(i) != nullptr)
        {
            strm
                << iris::replace_index(
                       R"(shadow{} = calculate_shadow(n, in.frag_pos_light_space{}, light_dir{}, )",
                       i)
                << texture_name(
                       static_cast<iris::TextureNode *>(
                           node.shadow_map_input(i))
                           ->texture(),
                       textures)
                << ");\n";
        }

        strm << iris::replace_index(
            R"(
                float diff{} = max(dot(n, light_dir{}), 0.0);
                float3 diffuse{} = (1.0  - shadow{}) * float3(diff{});
                diffuse += diffuse{};
                )",
            i);

        if (i >= directional_light_count)
        {
            strm << iris::replace_index(
                R"(
             float3 reflect_dir{} = reflect(-light_dir{}, n);
             float specular{} = pow(max(dot(view_dir, reflect_dir{}), 0.0), specular_power);
             specular += float3(specular{});)",
                i);
        }
    }
}

}

namespace iris
{

Compiler::Compiler(
    const RenderGraph *render_graph,
    const LightingRig *lighting_rig)
    : vertex_stream_()
    , fragment_stream_()
    , current_stream_(nullptr)
    , vertex_functions_()
    , fragment_functions_()
    , current_functions_(nullptr)
    , textures_()
    , lighting_rig_(lighting_rig)
{
    render_graph->render_node()->accept(*this);
}

void Compiler::visit(const RenderNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    vertex_stream_ << vertex_begin;
    build_tangent_values(vertex_stream_, lighting_rig_);

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    if (!node.is_depth_only())
    {
        fragment_stream_ << "float3 amb = float3("
                         << lighting_rig_->ambient_light.r << ", "
                         << lighting_rig_->ambient_light.g << ", "
                         << lighting_rig_->ambient_light.b << ");";

        fragment_stream_ << R"(
    float3 diffuse = float3(0);
    float3 specular = float3(0);
    float2 uv = in.tex.xy;
)";

        build_fragment_colour(fragment_stream_, node.colour_input(), this);
        build_specular_values(
            fragment_stream_,
            node.specular_power_input(),
            node.specular_amount_input(),
            this);
        build_normal(fragment_stream_, node.normal_input(), this);
        build_direction_values(
            fragment_stream_, node.normal_input() != nullptr, lighting_rig_);
        build_lighting_calculations(
            fragment_stream_, node, lighting_rig_, textures_);

        fragment_stream_ << R"(
    if (fragment_colour.a < 0.01)
    {
        discard_fragment();
    }

    return float4((amb + diffuse + (specular * specular_amount)) * fragment_colour.rgb, 1.0);
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
    const auto directional_light_count =
        lighting_rig_->directional_lights.size();
    const auto point_light_count = lighting_rig_->point_lights.size();

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
    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        stream << "float3 tangent_light_pos" << i << ";\n";
        stream << "float4 frag_pos_light_space" << i << ";\n";
    }
    stream << R"(
    float3 tangent_view_pos;
    float3 tangent_frag_pos;
} VertexOut;
)";
    stream << default_uniform << '\n';
    stream << directional_light_uniform << '\n';
    stream << point_light_uniform << '\n';

    for (const auto &function : vertex_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
 vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant DefaultUniform *uniform [[buffer(1)]],
    constant DirectionalLightUniform *directional_lights [[buffer(2)]],
    constant PointLightUniform *point_light [[buffer(3)]],
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
    const auto directional_light_count =
        lighting_rig_->directional_lights.size();
    const auto point_light_count = lighting_rig_->point_lights.size();

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
    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        stream << "float3 tangent_light_pos" << i << ";\n";
        stream << "float4 frag_pos_light_space" << i << ";\n";
    }
    stream << R"(
    float3 tangent_view_pos;
    float3 tangent_frag_pos;
} VertexOut;
)";
    stream << default_uniform << '\n';
    stream << directional_light_uniform << '\n';
    stream << point_light_uniform << '\n';

    for (const auto &function : fragment_functions_)
    {
        stream << function << '\n';
    }

    stream << R"(
fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    constant DefaultUniform *uniform [[buffer(0)]],
    constant DirectionalLightUniform *directional_lights [[buffer(1)]],
    constant PointLightUniform *point_light [[buffer(2)]])";

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
