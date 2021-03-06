#pragma once

#include <memory>
#include <vector>

#include "graphics/render_graph/node.h"

namespace iris
{

class Compiler;

/**
 * A RenderNode is an implementation of Node which represents the final node in
 * a render graph. Whilst it is the final node it is effectively the root of the
 * DAG and the whole graph can be traversed top-down from here. Only one should
 * be in each render_graph.
 */
class RenderNode : public Node
{
  public:
    /**
     * Construct a new RenderNode. All input nodes are initialised to nullptr,
     * which tells the compiler to default to values in the vertex data.
     */
    RenderNode();

    ~RenderNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(Compiler &compiler) const override;

    /**
     * Get colour input.
     *
     * @returns
     *   Colour input.
     */
    Node *colour_input() const;

    /**
     * Set colour input.
     *
     * @param input
     *   New input.
     */
    void set_colour_input(Node *input);

    /**
     * Set specular power input i.e. how shiny an object is.
     *
     * @returns
     *   Specular power input.
     */
    Node *specular_power_input() const;

    /**
     * Set specular power input.
     *
     * @param input
     *   New input.
     */
    void set_specular_power_input(Node *input);

    /**
     * Set specular amount input i.e. a scale from [0, 1] how much to apply the
     * specular power. Useful for specular maps or to disable specular.
     *
     * @returns
     *   Specular amount input.
     */
    Node *specular_amount_input() const;

    /**
     * Set specular amount input.
     *
     * @param input
     *   New input.
     */
    void set_specular_amount_input(Node *input);

    /**
     * Get normal input.
     *
     * @returns
     *   Colour input.
     */
    Node *normal_input() const;

    /**
     * Set normal input.
     *
     * @param input
     *   New input.
     */
    void set_normal_input(Node *input);

    /**
     * Get vertex position input.
     *
     * @returns
     *   Colour input.
     */
    Node *position_input() const;

    /**
     * Set vertex position input.
     *
     * @param input
     *   New input.
     */
    void set_position_input(Node *input);

    /**
     * Get the shadow map input node at a specified index.
     *
     * @param index
     *   Index of shadow map input.
     *
     * @returns
     *   Shadow map node at index if one exists, otherwise nullptr.
     */
    Node *shadow_map_input(std::size_t index) const;

    /**
     * Add a new shadow map node.
     *
     * @param input
     *   Shadow map input node.
     */
    void add_shadow_map_input(Node *input);

    /**
     * Is this render node a depth only render.
     *
     * @returns
     *   True if depth only render, otherwise false.
     */
    bool is_depth_only() const;

    /**
     * Set if this node is for a depth only render.
     *
     * @param depth_only
     *   New depth only value.
     */
    void set_depth_only(bool depth_only);

  private:
    /** Colour input. */
    Node *colour_input_;

    /** Specular power input. */
    Node *specular_power_input_;

    /** Specular amount input. */
    Node *specular_amount_input_;

    /** Normal input. */
    Node *normal_input_;

    /** Vertex position input. */
    Node *position_input_;

    /** Collection of shadow map inputs. */
    std::vector<Node *> shadow_map_inputs_;

    /** Is depth only render. */
    bool depth_only_;
};
}
