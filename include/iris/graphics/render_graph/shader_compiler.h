////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <set>
#include <sstream>
#include <stack>
#include <string>

#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/default_shader_languages.h"
#include "graphics/lights/light_type.h"
#include "graphics/render_graph/render_graph.h"

namespace inja
{
class Environment;
}

namespace iris
{

class RenderNode;
class SkyBoxNode;
class ColourNode;
class TextureNode;
class InvertNode;
class BlurNode;
class CompositeNode;
class BinaryOperatorNode;
class ConditionalNode;
class ComponentNode;
class CombineNode;
class SinNode;
template <typename>
class ValueNode;
class VertexNode;
class AmbientOcclusionNode;
class ColourAdjustNode;
class AntiAliasingNode;
class TimeNode;

/**
 * Interface for a class that compiles a RenderGraph into API specific shaders.
 * This treats the RenderGraph as an AST and parses it top-down.
 */
class ShaderCompiler
{
  public:
    ShaderCompiler(
        ShaderLanguage language,
        const RenderGraph *render_graph,
        LightType light_type,
        bool render_to_normal_target,
        bool render_to_position_target);

    ~ShaderCompiler();

    // visitor methods
    void visit(const RenderNode &node);
    void visit(const SkyBoxNode &node);
    void visit(const ColourNode &node);
    void visit(const TextureNode &node);
    void visit(const InvertNode &node);
    void visit(const BlurNode &node);
    void visit(const CompositeNode &node);
    void visit(const ValueNode<float> &node);
    void visit(const ValueNode<Vector3> &node);
    void visit(const ValueNode<Colour> &node);
    void visit(const BinaryOperatorNode &node);
    void visit(const ConditionalNode &node);
    void visit(const ComponentNode &node);
    void visit(const CombineNode &node);
    void visit(const SinNode &node);
    void visit(const VertexNode &node);
    void visit(const AmbientOcclusionNode &node);
    void visit(const ColourAdjustNode &node);
    void visit(const AntiAliasingNode &node);
    void visit(const TimeNode &node);

    /**
     * Get the compiled vertex shader.
     *
     * Compiled here means from the render graph to a string, not to an API
     * specific object on the hardware.
     *
     * @returns
     *   Vertex shader.
     */
    std::string vertex_shader() const;

    /**
     * Get the compiled fragment shader.
     *
     * Compiled here means from the render graph to a string, not to an API
     * specific object on the hardware.
     *
     * @returns
     *   Fragment shader.
     */
    std::string fragment_shader() const;

  private:
    ShaderLanguage language_;

    /** Stream for vertex shader. */
    std::stringstream vertex_stream_;

    /** Stream for fragment shader. */
    std::stringstream fragment_stream_;

    /** Collection of fragment functions. */
    std::set<std::string> fragment_functions_;

    /** Type of light to render with. */
    LightType light_type_;

    /** Flag indicating whether the shader should also write out screen space normals to a render texture. */
    bool render_to_normal_target_;

    /** Flag indicating whether the shader should also write out screen space positions to a render texture. */
    bool render_to_position_target_;

    /** Stack of streams to be used the render graph is traversed. */
    std::stack<std::stringstream> stream_stack_;

    bool is_vertex_shader_;

    /** inja library environemnt. */
    std::unique_ptr<inja::Environment> env_;
};
}