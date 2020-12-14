#pragma once

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
 * This class automatically creates and managed a RenderNode.
 */
class RenderGraph
{
  public:
    // helper trait
    template <class T>
    using is_node = std::enable_if_t<
        std::is_base_of_v<Node, T> && !std::is_same_v<RenderNode, T>>;

    /**
     * Create a new RenderGraph.
     */
    RenderGraph();

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
    T *create(Args &&... args)
    {
        auto node = std::make_unique<T>(std::forward<Args>(args)...);
        return static_cast<T *>(add(std::move(node)));
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
    /** Collection of nodes in graph. */
    std::vector<std::unique_ptr<Node>> nodes_;
};
}
