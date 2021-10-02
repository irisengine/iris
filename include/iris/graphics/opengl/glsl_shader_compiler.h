#pragma once

#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "graphics/lights/light_type.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/texture.h"
#include "graphics/vertex_attributes.h"

namespace iris
{

class RenderNode;
class ColourNode;
class TextureNode;
class InvertNode;
class BlurNode;
class CompositeNode;
class VertexPositionNode;
class ArithmeticNode;
class ConditionalNode;
class ComponentNode;
class CombineNode;
class SinNode;
template <typename>
class ValueNode;

/**
 * Implementation of ShaderCompiler for GLSL.
 */
class GLSLShaderCompiler : public ShaderCompiler
{
  public:
    /**
     * Construct a new GLSLSHaderCompiler.
     *
     * @param render_graph
     *   RenderGraph to cpmpile into GLSL.
     *
     * @param light_type
     *   The type of light to render with.
     */
    GLSLShaderCompiler(const RenderGraph *render_graph, LightType light_type);

    ~GLSLShaderCompiler() override = default;

    // visitor methods
    void visit(const RenderNode &node) override;
    void visit(const ColourNode &node) override;
    void visit(const TextureNode &node) override;
    void visit(const InvertNode &node) override;
    void visit(const BlurNode &node) override;
    void visit(const CompositeNode &node) override;
    void visit(const VertexPositionNode &node) override;
    void visit(const ValueNode<float> &node) override;
    void visit(const ArithmeticNode &node) override;
    void visit(const ConditionalNode &node) override;
    void visit(const ComponentNode &node) override;
    void visit(const CombineNode &node) override;
    void visit(const SinNode &node) override;

    /**
     * Get the compiled vertex shader.
     *
     * Compiled here means from the render graph to a string, not to an API
     * specific object on the hardware.
     *
     * @returns
     *   Vertex shader.
     */
    std::string vertex_shader() const override;

    /**
     * Get the compiled fragment shader.
     *
     * Compiled here means from the render graph to a string, not to an API
     * specific object on the hardware.
     *
     * @returns
     *   Fragment shader.
     */
    std::string fragment_shader() const override;

    /**
     * Collection of textures needed for the shaders.
     *
     * @returns
     *   Collection of Textures.
     */
    std::vector<Texture *> textures() const override;

  private:
    /** Stream for vertex shader. */
    std::stringstream vertex_stream_;

    /** Stream for fragment shader. */
    std::stringstream fragment_stream_;

    /** Pointer to the current shader stream. */
    std::stringstream *current_stream_;

    /** Collection of vertex functions. */
    std::set<std::string> vertex_functions_;

    /** Collection of fragment functions. */
    std::set<std::string> fragment_functions_;

    /** Pointer to current function collection. */
    std::set<std::string> *current_functions_;

    /** Textures needed for shaders. */
    std::vector<Texture *> textures_;

    /** Type of light to render with. */
    LightType light_type_;
};
}
