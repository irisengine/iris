#include "graphics/render_graph/compiler.h"

#include <sstream>
#include <string>

#include "core/exception.h"
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
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_position_node.h"
#include "graphics/texture.h"

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
)";

static constexpr auto vertex_out = R"(
out vec4 fragPos;
out vec2 texCoord;
out vec4 col;
out vec4 norm;
out vec3 tangent_frag_pos;
)";

static constexpr auto fragment_in = R"(
in vec2 texCoord;
in vec4 col;
in vec4 norm;
in vec4 fragPos;
in vec3 tangent_frag_pos;
)";

static constexpr auto fragment_out = R"(
out vec4 outColor;
)";

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

    vertex_stream_ << "void main()\n{\n";
    vertex_stream_ << R"(
    mat4 bone_transform = bones[bone_ids[0]] * bone_weights[0];
    bone_transform += bones[bone_ids[1]] * bone_weights[1];
    bone_transform += bones[bone_ids[2]] * bone_weights[2];
    bone_transform += bones[bone_ids[3]] * bone_weights[3];

    vec3 T = normalize(vec3(normal_matrix * bone_transform * tangent));
    vec3 B = normalize(vec3(normal_matrix * bone_transform * bitangent));
    vec3 N = normalize(vec3(normal_matrix * bone_transform * normal));
    mat3 tbn = mat3(T, B, N);

    col = colour;
    norm = normal_matrix * bone_transform * normal;
    texCoord = vec2(tex.x, tex.y);
    fragPos = model * bone_transform * position;
    gl_Position = projection * view * fragPos;

)";

    for (auto i = 0u; i < lights_.size(); ++i)
    {
        vertex_stream_ << "frag_pos_light_space" << i << " = light_proj" << i
                       << " * light_view" << i << " * fragPos;\n";

        vertex_stream_ << "tangent_light_pos" << i << " = tbn * light" << i
                       << ";\n";
    }

    vertex_stream_ << "tangent_frag_pos = tbn * fragPos.xyz;\n";
    vertex_stream_ << "}";

    current_stream_ = &fragment_stream_;
    current_functions_ = &fragment_functions_;

    fragment_stream_ << "void main()\n{\n";

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
                fragment_stream_ << "vec3 light_dir" << i
                                 << " = normalize(-light" << i << ");\n";
            }
        }
        else
        {
            fragment_stream_ << "vec3(";
            normal_input->accept(*this);
            fragment_stream_ << ");\n";
            fragment_stream_ << "    n = n * 2.0 - 1.0;\n";
            for (auto i = 0u; i < lights_.size(); ++i)
            {
                fragment_stream_ << "vec3 light_dir" << i
                                 << "= normalize(-tangent_light_pos" << i
                                 << ");\n";
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
                    << "vec3 proj_coord" << i << " = frag_pos_light_space" << i
                    << ".xyz / frag_pos_light_space" << i << ".w;\n";
                fragment_stream_ << "proj_coord" << i << " = proj_coord" << i
                                 << " * 0.5 + 0.5;\n";
                fragment_stream_
                    << "float closest_depth" << i << " = texture("
                    << texture_name(
                           static_cast<TextureNode *>(node.shadow_map_input(i))
                               ->texture(),
                           textures_)
                    << ", proj_coord" << i << ".xy).r;\n";
                fragment_stream_ << "float current_depth" << i
                                 << " = proj_coord" << i << ".z;\n";
                fragment_stream_ << "float bias" << i
                                 << " = max(0.05 * (1.0 - dot(n, light_dir" << i
                                 << ")), 0.005);\n";
                fragment_stream_ << "shadow" << i << " = current_depth" << i
                                 << " - bias" << i << " > closest_depth" << i
                                 << " ? 1.0 : 0.0;\n";
                fragment_stream_ << "if(proj_coord" << i << ".z > 1.0) {shadow"
                                 << i << " = 0.0;}\n";
            }

            fragment_stream_ << "float diff" << i << " = max(dot(n, light_dir"
                             << i << "), 0.0);\n";
            fragment_stream_ << "vec3 diffuse" << i << " = (1.0  - shadow" << i
                             << ") * diff" << i << " * vec3(1);\n";
            fragment_stream_ << "diffuse += diffuse" << i << ";\n";
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
                     << ", texCoord)";
}

void Compiler::visit(const InvertNode &node)
{
    current_functions_->emplace(R"(
vec4 invert(vec4 colour)
{
    return vec4(vec3(1.0 - colour), 1.0);
})");

    *current_stream_ << "invert(";
    node.input_node()->accept(*this);
    *current_stream_ << ")";
}

void Compiler::visit(const BlurNode &node)
{
    current_functions_->emplace(R"(
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
})");

    *current_stream_ << "blur("
                     << texture_name(node.input_node()->texture(), textures_)
                     << ", texCoord)";
}

void Compiler::visit(const CompositeNode &node)
{
    current_functions_->emplace(R"(
vec4 composite(vec4 colour1, vec4 colour2, vec4 depth1, vec4 depth2, vec2 tex_coord)
{
    vec4 colour = colour2;

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
    *current_stream_ << ", texCoord)";
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
