#pragma once

#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "graphics/render_graph/render_graph.h"
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
class ComponentNode;
class CombineNode;
class SinNode;
template <typename>
class ValueNode;

/**
 * This class compiles a RenderGraph into API specific shaders. This treats the
 * RenderGraph as an AST and parses it top-down.
 */
class Compiler
{
  public:
    /**
     * Create a new Compiler.
     *
     * @param render_graph
     *   Graph to compiler.
     */
    Compiler(const RenderGraph &render_graph);

    // visitor methods
    void visit(const RenderNode &node);
    void visit(const ColourNode &node);
    void visit(const TextureNode &node);
    void visit(const InvertNode &node);
    void visit(const BlurNode &node);
    void visit(const CompositeNode &node);
    void visit(const VertexPositionNode &node);
    void visit(const ValueNode<float> &node);
    void visit(const ArithmeticNode &node);
    void visit(const ComponentNode &node);
    void visit(const CombineNode &node);
    void visit(const SinNode &node);

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

    /**
     * Collection of textures needed for the shaders.
     *
     * @returns
     *   Collection of Textures.
     */
    std::vector<Texture *> textures() const;

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
};
}