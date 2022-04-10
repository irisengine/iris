////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/lights/light_type.h"

namespace iris
{

class RenderNode;
class SkyBoxNode;
class ColourNode;
class TextureNode;
class InvertNode;
class BlurNode;
class CompositeNode;
class ArithmeticNode;
class ConditionalNode;
class ComponentNode;
class CombineNode;
class SinNode;
template <typename>
class ValueNode;
class VertexNode;
class AmbientOcclusionNode;
class ToneMapNode;
class GammaCorrectNode;
class AntiAliasingNode;

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
    virtual void visit(const SkyBoxNode &node) = 0;
    virtual void visit(const ColourNode &node) = 0;
    virtual void visit(const TextureNode &node) = 0;
    virtual void visit(const InvertNode &node) = 0;
    virtual void visit(const BlurNode &node) = 0;
    virtual void visit(const CompositeNode &node) = 0;
    virtual void visit(const ValueNode<float> &node) = 0;
    virtual void visit(const ValueNode<Vector3> &node) = 0;
    virtual void visit(const ValueNode<Colour> &node) = 0;
    virtual void visit(const ArithmeticNode &node) = 0;
    virtual void visit(const ConditionalNode &node) = 0;
    virtual void visit(const ComponentNode &node) = 0;
    virtual void visit(const CombineNode &node) = 0;
    virtual void visit(const SinNode &node) = 0;
    virtual void visit(const VertexNode &node) = 0;
    virtual void visit(const AmbientOcclusionNode &node) = 0;
    virtual void visit(const ToneMapNode &node) = 0;
    virtual void visit(const GammaCorrectNode &node) = 0;
    virtual void visit(const AntiAliasingNode &node) = 0;

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
};
}