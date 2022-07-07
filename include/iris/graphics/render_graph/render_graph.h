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

/**
 * This class encapsulates a render graph - a series of connected nodes that
 * can be compiled into API specific shaders.
 *
 * This class automatically creates and manages a RenderNode.
 */
class RenderGraph
{
  public:
    // helper trait
    template <class T>
    using is_node = std::enable_if_t<std::is_base_of_v<Node, T> && !std::is_same_v<RenderNode, T>>;

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
    template <class T, class... Args, typename = is_node<T>>
    T *create(Args &&...args)
    {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        return static_cast<T *>(add(std::move(node)));
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
    template <class T, class... Args, typename = is_node<T>>
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

  private:
    // friend to allow only the RenderPipeline to create
    friend class RenderPipeline;

    /**
     * Create a new RenderGraph.
     */
    RenderGraph();

    /** Collection of nodes in graph. */
    std::vector<std::unique_ptr<Node>> nodes_;
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
