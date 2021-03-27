#include "graphics/render_graph/compiler.h"

#include <sstream>
#include <string>

#include "core/exception.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/opengl/compiler_strings.h"
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

    return "texture" + std::to_string(texture->texture_id());
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
            "frag_pos_light_space{} = light_proj{} * light_view{} * frag_pos;\n"
            "tangent_light_pos{} = tbn * light{};\n",
            i);
    }

    for (auto i = 0u; i < point_light_count; ++i)
    {
        strm << iris::replace_index(
            "tangent_light_pos{} = tbn * light{};\n",
            i + directional_light_count);
    }

    strm << "tangent_view_pos = tbn * camera;";
    strm << "tangent_frag_pos = tbn * frag_pos.xyz; }";
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
    strm << "vec4 fragment_colour = ";
    visit_or_default(strm, colour, visitor, "col");
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
    strm << "vec3 n = ";

    if (normal == nullptr)
    {
        strm << "normalize(norm.xyz);\n";
    }
    else
    {
        strm << "vec3(";
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
                 ? "vec3 light_dir{} = normalize(-light{});\n"
                 : "vec3 light_dir{} = normalize(-tangent_light_pos{});\n"),
            i);
    }

    for (auto i = 0u; i < lighting_rig->point_lights.size(); ++i)
    {
        strm << iris::replace_index(
            (!has_normal_input
                 ? "vec3 light_dir{} = normalize(light{} - frag_pos.xyz);\n"
                 : "vec3 light_dir{} = normalize(tangent_light_pos{} - "
                   "tangent_frag_pos.xyz);\n"),
            i + lighting_rig->directional_lights.size());
    }

    strm
        << (!has_normal_input
                ? "vec3 view_dir = normalize(camera - frag_pos.xyz);\n"
                : "vec3 view_dir = normalize(tangent_view_pos - "
                  "tangent_frag_pos);\n");
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
                       R"(shadow{} = calculate_shadow(n, frag_pos_light_space{}, light_dir{}, )",
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
                vec3 diffuse{} = (1.0  - shadow{}) * vec3(diff{});
                diffuse += diffuse{};
                )",
            i);

        if (i >= directional_light_count)
        {
            strm << iris::replace_index(
                R"(
             vec3 reflect_dir{} = reflect(-light_dir{}, n);
             float specular{} = pow(max(dot(view_dir, reflect_dir{}), 0.0), specular_power);
             specular += specular{};)",
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

    vertex_stream_ << " void main()\n{\n";
    vertex_stream_ << vertex_begin;

    build_tangent_values(vertex_stream_, lighting_rig_);

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    fragment_stream_ << "void main()\n{\n";

    if (!node.is_depth_only())
    {
        fragment_stream_ << "vec3 amb = vec3(" << lighting_rig_->ambient_light.r
                         << ", " << lighting_rig_->ambient_light.g << ", "
                         << lighting_rig_->ambient_light.b << ");";
        fragment_stream_ << "vec3 diffuse = vec3(0);\n";
        fragment_stream_ << "vec3 specular = vec3(0);\n";

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
        outColor = vec4((amb + diffuse + (specular * specular_amount)) * fragment_colour.rgb, 1.0);

        if (fragment_colour.a < 0.01)
        {
            discard;
        }
)";
    }
    fragment_stream_ << "}";
}

void Compiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "vec4(" << colour.r << ", " << colour.g << ", "
                     << colour.b << ", " << colour.a << ")";
}

void Compiler::visit(const TextureNode &node)
{
    *current_stream_ << "texture(" << texture_name(node.texture(), textures_);

    if (node.texture()->flip())
    {
        *current_stream_ << ", vec2(tex_coord.s, 1.0 - tex_coord.t))";
    }
    else
    {
        *current_stream_ << ", tex_coord)";
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
                     << texture_name(node.input_node()->texture(), textures_)
                     << ", tex_coord)";
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
    *current_stream_ << ", tex_coord)";
}

void Compiler::visit(const VertexPositionNode &)
{
    *current_stream_ << "position";
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
    *current_stream_ << "vec4(";
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
    stream << layouts << '\n';

    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        stream << "out vec3 tangent_light_pos" << i << ";\n";
        stream << "out vec4 frag_pos_light_space" << i << ";\n";
    }

    stream << uniforms << '\n';

    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        stream << "uniform vec3 light" << i << ";\n";

        if (i < directional_light_count)
        {
            stream << "uniform mat4 light_proj" << i << ";\n";
            stream << "uniform mat4 light_view" << i << ";\n";
        }
    }

    for (auto i = 0u; i < textures_.size(); ++i)
    {
        stream << "uniform sampler2D texture" << i << ";\n";
    }

    stream << vertex_out << '\n';

    for (const auto &function : vertex_functions_)
    {
        stream << function << '\n';
    }

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
    stream << uniforms << '\n';

    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        stream << "uniform vec3 light" << i << ";\n";

        if (i < directional_light_count)
        {
            stream << "uniform mat4 light_proj" << i << ";\n";
            stream << "uniform mat4 light_view" << i << ";\n";
        }
    }

    for (const auto *texture : textures_)
    {
        stream << "uniform sampler2D texture" << texture->texture_id() << ";\n";
    }

    stream << fragment_in << '\n';

    for (auto i = 0u; i < directional_light_count + point_light_count; ++i)
    {
        stream << "in vec3 tangent_light_pos" << i << ";\n";
        stream << "in vec4 frag_pos_light_space" << i << ";\n";
    }

    stream << fragment_out << '\n';

    for (const auto &function : fragment_functions_)
    {
        stream << function << '\n';
    }

    stream << fragment_stream_.str() << '\n';

    return stream.str();
}

std::vector<Texture *> Compiler::textures() const
{
    return textures_;
}
}
