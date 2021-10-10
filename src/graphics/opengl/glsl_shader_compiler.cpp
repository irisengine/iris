////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/glsl_shader_compiler.h"

#include <sstream>
#include <string>

#include "core/colour.h"
#include "core/exception.h"
#include "core/vector3.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/opengl/compiler_strings.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/blur_node.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/combine_node.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/composite_node.h"
#include "graphics/render_graph/conditional_node.h"
#include "graphics/render_graph/invert_node.h"
#include "graphics/render_graph/post_processing_node.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/sin_node.h"
#include "graphics/render_graph/texture_node.h"
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
    std::size_t id = 0u;

    // id will be index into the textures collection

    const auto find =
        std::find(std::cbegin(textures), std::cend(textures), texture);

    if (find != std::cend(textures))
    {
        id = std::distance(std::cbegin(textures), find);
    }
    else
    {
        id = textures.size();
        textures.emplace_back(texture);
    }

    return "texture" + std::to_string(id);
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
    iris::GLSLShaderCompiler *visitor,
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
 * @param light_type
 *   Type if light to render with.
 */
void build_tangent_values(std::stringstream &strm, iris::LightType light_type)
{
    if (light_type == iris::LightType::DIRECTIONAL)
    {
        strm << "frag_pos_light_space = light_projection * light_view * "
                "frag_pos;\n";
    }

    strm << "tangent_light_pos = tbn * light_position.xyz;\n";
    strm << "tangent_view_pos = tbn * camera_.xyz;\n";
    strm << "tangent_frag_pos = tbn * frag_pos.xyz;\n";
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
    iris::GLSLShaderCompiler *visitor)
{
    strm << "vec4 fragment_colour = ";
    visit_or_default(strm, colour, visitor, "col");
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
    iris::GLSLShaderCompiler *visitor)
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

}

namespace iris
{
GLSLShaderCompiler::GLSLShaderCompiler(
    const RenderGraph *render_graph,
    LightType light_type)
    : vertex_stream_()
    , fragment_stream_()
    , current_stream_(nullptr)
    , vertex_functions_()
    , fragment_functions_()
    , current_functions_(nullptr)
    , textures_()
    , light_type_(light_type)
{
    render_graph->render_node()->accept(*this);
}

void GLSLShaderCompiler::visit(const RenderNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    // build vertex shader

    vertex_stream_ << " void main()\n{\n";
    vertex_stream_ << vertex_begin;
    build_tangent_values(vertex_stream_, light_type_);
    vertex_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    fragment_stream_ << "void main()\n{\n";

    if (!node.is_depth_only())
    {
        // build basic values
        build_fragment_colour(*current_stream_, node.colour_input(), this);
        build_normal(*current_stream_, node.normal_input(), this);

        // depending on the light type depends on how we interpret the light
        // uniform and how we calculate lighting
        switch (light_type_)
        {
            case LightType::AMBIENT:
                *current_stream_ << R"(
            outColour = light_colour * fragment_colour;)";
                break;
            case LightType::DIRECTIONAL:
                *current_stream_ << "vec3 light_dir = ";
                *current_stream_
                    << (node.normal_input() == nullptr
                            ? "normalize(-light_position.xyz);\n"
                            : "normalize(-tangent_light_pos.xyz);\n");

                *current_stream_ << "float shadow = 0.0;\n";
                *current_stream_ <<
                    R"(shadow = calculate_shadow(n, frag_pos_light_space, light_position.xyz, g_shadow_map);
                )";

                *current_stream_ << R"(
                 float diff = (1.0 - shadow) * max(dot(n, light_dir), 0.0);
                 vec3 diffuse = vec3(diff);

                 outColour = vec4(diffuse * fragment_colour.xyz, 1.0);
                )";
                break;
            case LightType::POINT:
                *current_stream_ << "vec3 light_dir = ";
                *current_stream_
                    << (node.normal_input() == nullptr
                            ? "normalize(light_position.xyz - frag_pos.xyz);\n"
                            : "normalize(tangent_light_pos.xyz - "
                              "tangent_frag_pos.xyz);\n");
                *current_stream_ << R"(
                float distance  = length(light_position.xyz - frag_pos.xyz);
                float constant = light_attenuation[0];
                float linear = light_attenuation[1];
                float quadratic = light_attenuation[2];
                float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    

                float diff = max(dot(n, light_dir), 0.0);
                vec3 diffuse = vec3(diff);
                
                outColour = vec4(diffuse * light_colour.xyz * fragment_colour.xyz * vec3(attenuation), 1.0);
                )";
                break;
        }

        *current_stream_ << R"(
        if (fragment_colour.a < 0.01)
        {
            discard;
        }
)";
    }
    fragment_stream_ << "}";
}

void GLSLShaderCompiler::visit(const PostProcessingNode &node)
{
    current_stream_ = &vertex_stream_;
    current_functions_ = &vertex_functions_;

    current_functions_->emplace(bone_transform_function);
    current_functions_->emplace(tbn_function);

    // build vertex shader

    vertex_stream_ << " void main()\n{\n";
    vertex_stream_ << vertex_begin;
    vertex_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    current_functions_->emplace(shadow_function);

    // build fragment shader

    fragment_stream_ << "void main()\n{\n";

    // build basic values
    build_fragment_colour(*current_stream_, node.colour_input(), this);

    *current_stream_ << R"(
        vec3 mapped = fragment_colour.rgb / (fragment_colour.rgb + vec3(1.0));
        mapped = pow(mapped, vec3(1.0 / 2.2));

        outColour = vec4(mapped, 1.0);
    })";
}

void GLSLShaderCompiler::visit(const ColourNode &node)
{
    const auto colour = node.colour();
    *current_stream_ << "vec4(" << colour.r << ", " << colour.g << ", "
                     << colour.b << ", " << colour.a << ")";
}

void GLSLShaderCompiler::visit(const TextureNode &node)
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

void GLSLShaderCompiler::visit(const InvertNode &node)
{
    current_functions_->emplace(invert_function);

    *current_stream_ << "invert(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void GLSLShaderCompiler::visit(const BlurNode &node)
{
    current_functions_->emplace(blur_function);

    *current_stream_ << "blur("
                     << texture_name(node.input_node()->texture(), textures_)
                     << ", tex_coord)";
}

void GLSLShaderCompiler::visit(const CompositeNode &node)
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

void GLSLShaderCompiler::visit(const VertexPositionNode &)
{
    *current_stream_ << "position";
}

void GLSLShaderCompiler::visit(const ValueNode<float> &node)
{
    *current_stream_ << std::to_string(node.value());
}

void GLSLShaderCompiler::visit(const ValueNode<Vector3> &node)
{
    *current_stream_ << "vec3(" << node.value().x << ", " << node.value().y
                     << ", " << node.value().z << ")";
}

void GLSLShaderCompiler::visit(const ValueNode<Colour> &node)
{
    *current_stream_ << "vec4(" << node.value().g << ", " << node.value().g
                     << ", " << node.value().b << ", " << node.value().a << ")";
}

void GLSLShaderCompiler::visit(const ArithmeticNode &node)
{
    *current_stream_ << "(";

    if (node.arithmetic_operator() == ArithmeticOperator::DOT)
    {
        *current_stream_ << "dot(";
        node.value1()->accept(*this);
        *current_stream_ << ", ";
        node.value2()->accept(*this);
        *current_stream_ << ")";
    }
    else
    {
        node.value1()->accept(*this);
        switch (node.arithmetic_operator())
        {
            case ArithmeticOperator::ADD: *current_stream_ << " + "; break;
            case ArithmeticOperator::SUBTRACT: *current_stream_ << " - "; break;
            case ArithmeticOperator::MULTIPLY: *current_stream_ << " * "; break;
            case ArithmeticOperator::DIVIDE: *current_stream_ << " / "; break;
            default: throw Exception("unknown arithmetic operator");
        }
        node.value2()->accept(*this);
    }

    *current_stream_ << ")";
}

void GLSLShaderCompiler::visit(const ConditionalNode &node)
{
    *current_stream_ << "(";
    node.input_value1()->accept(*this);

    switch (node.conditional_operator())
    {
        case ConditionalOperator::GREATER: *current_stream_ << " > "; break;
    }

    node.input_value2()->accept(*this);

    *current_stream_ << " ? ";
    node.output_value1()->accept(*this);
    *current_stream_ << " : ";
    node.output_value2()->accept(*this);
    *current_stream_ << ")";
}

void GLSLShaderCompiler::visit(const ComponentNode &node)
{
    node.input_node()->accept(*this);
    *current_stream_ << "." << node.component();
}

void GLSLShaderCompiler::visit(const CombineNode &node)
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

void GLSLShaderCompiler::visit(const SinNode &node)
{
    *current_stream_ << "sin(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

std::string GLSLShaderCompiler::vertex_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << layouts << '\n';
    stream << uniforms << '\n';

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

std::string GLSLShaderCompiler::fragment_shader() const
{
    std::stringstream stream{};

    stream << preamble << '\n';
    stream << uniforms << '\n';
    stream << "uniform sampler2D g_shadow_map;\n";

    for (auto i = 0u; i < textures_.size(); ++i)
    {
        stream << "uniform sampler2D texture" << i << ";\n";
    }

    stream << fragment_in << '\n';
    stream << fragment_out << '\n';

    for (const auto &function : fragment_functions_)
    {
        stream << function << '\n';
    }

    stream << fragment_stream_.str() << '\n';

    return stream.str();
}

std::vector<Texture *> GLSLShaderCompiler::textures() const
{
    return textures_;
}
}
