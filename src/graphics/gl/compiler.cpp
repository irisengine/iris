#include "graphics/render_graph/compiler.h"

#include <sstream>
#include <string>

#include "core/exception.h"
#include "graphics/gl/compiler_strings.h"
#include "graphics/light.h"
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

namespace iris
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
}

namespace iris
{
Compiler::Compiler(
    const RenderGraph &render_graph,
    const std::vector<Light *> &lights)
    : vertex_stream_()
    , fragment_stream_()
    , vertex_functions_()
    , fragment_functions_()
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
    void main()
    {
    mat4 bone_transform = calculate_bone_transform(bone_ids, bone_weights);
    mat3 tbn = calculate_tbn(bone_transform);

    col = colour;
    norm = normal_matrix * bone_transform * normal;
    tex_coord = vec2(tex.x, tex.y);
    frag_pos = model * bone_transform * position;
    gl_Position = projection * view * frag_pos;
)";

    for (auto i = 0u; i < lights_.size(); ++i)
    {
        vertex_stream_ << replace_index(
            R"(
            frag_pos_light_space{} = light_proj{} * light_view{} * frag_pos;
            tangent_light_pos{} = tbn * light{};
            )",
            i);
    }

    vertex_stream_ << "tangent_frag_pos = tbn * frag_pos.xyz; }";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    fragment_stream_ << R"(void main() {)";

    if (!node.is_depth_only())
    {
        fragment_stream_ << "vec4 fragment_colour = ";

        auto colour_input = node.colour_input();
        if (colour_input == nullptr)
        {
            fragment_stream_ << "col;\n";
        }
        else
        {
            colour_input->accept(*this);
            fragment_stream_ << ";\n";
        }
        fragment_stream_ << "vec3 n = ";

        auto normal_input = node.normal_input();
        if (normal_input == nullptr)
        {
            fragment_stream_ << "normalize(norm.xyz);\n";

            for (auto i = 0u; i < lights_.size(); ++i)
            {
                fragment_stream_ << replace_index(
                    R"(
                    vec3 light_dir{} = normalize(-light{});
                    )",
                    i);
            }
        }
        else
        {
            fragment_stream_ << "vec3(";
            normal_input->accept(*this);
            fragment_stream_ << ");\n";
            fragment_stream_ << "n = n * 2.0 - 1.0;\n";
            for (auto i = 0u; i < lights_.size(); ++i)
            {
                fragment_stream_ << replace_index(
                    R"(
                    vec3 light_dir{} = normalize(-tangent_light_pos{});
                    )",
                    i);
            }
        }

        if (lights_.empty())
        {
            fragment_stream_ << "vec3 amb = vec3(1.0);\n";
        }
        else
        {
            fragment_stream_ << "vec3 amb = vec3(0.15);\n";
        }

        fragment_stream_ << "vec3 diffuse = vec3(0);\n";

        for (auto i = 0u; i < lights_.size(); ++i)
        {
            fragment_stream_ << "float shadow" << i << " = 0.0;\n";
            if (node.shadow_map_input(i) != nullptr)
            {
                fragment_stream_
                    << replace_index(
                           R"(shadow{} = calculate_shadow(n, frag_pos_light_space{}, light_dir{}, )",
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
                vec3 diffuse{} = (1.0  - shadow{}) * vec3(diff{});
                diffuse += diffuse{};)",
                i);
        }

        fragment_stream_ << R"(
        outColor = vec4((amb + diffuse) * fragment_colour.rgb, 1.0);

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
    *current_stream_ << "texture(" << texture_name(node.texture(), textures_)
                     << ", tex_coord)";
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
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << layouts << '\n';

    for (auto i = 0u; i < lights_.size(); ++i)
    {
        stream << "out vec3 tangent_light_pos" << i << ";\n";
        stream << "out vec4 frag_pos_light_space" << i << ";\n";
    }

    stream << uniforms << '\n';

    for (auto i = 0u; i < lights_.size(); ++i)
    {
        stream << "uniform vec3 light" << i << ";\n";
        stream << "uniform mat4 light_proj" << i << ";\n";
        stream << "uniform mat4 light_view" << i << ";\n";
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
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << uniforms << '\n';

    for (auto i = 0u; i < lights_.size(); ++i)
    {
        stream << "uniform vec3 light" << i << ";\n";
        stream << "uniform mat4 light_proj" << i << ";\n";
        stream << "uniform mat4 light_view" << i << ";\n";
    }

    for (const auto *texture : textures_)
    {
        stream << "uniform sampler2D texture" << texture->texture_id() << ";\n";
    }

    stream << fragment_in << '\n';

    for (auto i = 0u; i < lights_.size(); ++i)
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
