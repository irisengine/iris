////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/render_node.h"

namespace iris
{

template <class T>
concept IsNode = std::is_base_of_v<Node, T> && !std::is_same_v<RenderNode, T>;

/**
 * Enumeration of possible variable types.
 */
enum class VariableNodeType
{
    FLOAT,
    VEC3,
    VEC4
};

/**
 * Struct encapsulating all the data needed for a variable.
 */
struct RenderGraphVariable
{
    /** User name of variable. */
    std::string name;

    /** Type of variable. */
    VariableNodeType type;

    /** Node which when parsed will create the value for the variable. */
    Node *value;

    /** Whether this is declaring and setting (true) or just setting (false) a variable. */
    bool is_declaration;
};

/**
 * This class encapsulates a render graph - a series of connected nodes that
 * can be compiled into API specific shaders.
 *
 * This class automatically creates and manages a RenderNode.
 */
class RenderGraph
{
  public:
    /**
     * Get the RenderNode i.e. the root of the graph.
     *
     * @returns
     *   Render node.
     */
    RenderNode *render_node() const;

    /**
     * Create a Node and add it to the graph. Uses perfect forwarding to pass
     * along arguments.
     *
     * @param args
     *   Arguments for Node.
     *
     * @returns
     *   A pointer to the newly created Node.
     */
    template <IsNode T, class... Args>
    T *create(Args &&...args)
    {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        return static_cast<T *>(add(std::move(node)));
    }

    /**
     * Create a new variable. This will be available to both vertex and fragment shaders, but will only be included if
     * it is actually used.
     *
     * @param name
     *   User name of variable.
     *
     * @param type
     *   Type of variable
     *
     * @param is_declaration
     *   Whether this is declaring and setting (true) or just setting (false) a variable.
     *
     * @param args
     *   Pack of arguments used to construct node of type T.
     */
    template <IsNode T, class... Args>
    void create_variable(const std::string &name, VariableNodeType type, bool is_declaration, Args &&...args)
    {
        auto *node = create<T>(std::forward<Args>(args)...);
        variables_.push_back({.name = name, .type = type, .value = node, .is_declaration = is_declaration});
    }

    /**
     * Set the render node.
     *
     * @param args
     *   Arguments for Node.
     *
     * @returns
     *   A pointer to the newly created Node.
     */
    template <IsNode T, class... Args>
    RenderNode *set_render_node(Args &&...args)
    {
        nodes_[0] = std::make_unique<T>(std::forward<Args>(args)...);
        return render_node();
    }

    /**
     * Add a Node to the graph.
     *
     * @param node
     *   Node to add.
     *
     * @returns
     *   Pointer to the added node.
     */
    Node *add(std::unique_ptr<Node> node);

    /**
     * Get all variables.
     *
     * @returns
     *   Collection of variables.
     */
    std::vector<RenderGraphVariable> variables() const;

  private:
    // friend to allow only the RenderPipeline to create
    friend class RenderPipeline;

    /**
     * Create a new RenderGraph.
     */
    RenderGraph();

    /** Collection of nodes in graph. */
    std::vector<std::unique_ptr<Node>> nodes_;

    /** Collection of variables. */
    std::vector<RenderGraphVariable> variables_;
};

}

// specialise std::hash for RenderGraph pointer
namespace std
{

template <>
struct hash<iris::RenderGraph *>
{
    size_t operator()(const iris::RenderGraph *rg) const
    {
        return std::hash<iris::Node *>{}(rg->render_node());
    }
};

}
