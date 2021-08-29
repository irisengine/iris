#pragma once

#include <string>
#include <vector>

#include "graphics/lights/light_type.h"
#include "graphics/texture.h"

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
 * Interface for a class that compiles a RenderGraph into API specific shaders.
 * This treats the RenderGraph as an AST and parses it top-down.
 */
class ShaderCompiler
{
  public:
    virtual ~ShaderCompiler() = default;

    // visitor methods
    virtual void visit(const RenderNode &node) = 0;
    virtual void visit(const ColourNode &node) = 0;
    virtual void visit(const TextureNode &node) = 0;
    virtual void visit(const InvertNode &node) = 0;
    virtual void visit(const BlurNode &node) = 0;
    virtual void visit(const CompositeNode &node) = 0;
    virtual void visit(const VertexPositionNode &node) = 0;
    virtual void visit(const ValueNode<float> &node) = 0;
    virtual void visit(const ArithmeticNode &node) = 0;
    virtual void visit(const ComponentNode &node) = 0;
    virtual void visit(const CombineNode &node) = 0;
    virtual void visit(const SinNode &node) = 0;

    /**
     * Get the compiled vertex shader.
     *
     * Compiled here means from the render graph to a string, not to an API
     * specific object on the hardware.
     *
     * @returns
     *   Vertex shader.
     */
    virtual std::string vertex_shader() const = 0;

    /**
     * Get the compiled fragment shader.
     *
     * Compiled here means from the render graph to a string, not to an API
     * specific object on the hardware.
     *
     * @returns
     *   Fragment shader.
     */
    virtual std::string fragment_shader() const = 0;

    /**
     * Collection of textures needed for the shaders.
     *
     * @returns
     *   Collection of Textures.
     */
    virtual std::vector<Texture *> textures() const = 0;
};
}