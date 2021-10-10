////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/render_graph.h"

#include <memory>

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/render_node.h"

namespace iris
{

RenderGraph::RenderGraph()
    : nodes_()
{
    nodes_.emplace_back(std::make_unique<RenderNode>());
}

RenderNode *RenderGraph::render_node() const
{
    return static_cast<RenderNode *>(nodes_.front().get());
}

Node *RenderGraph::add(std::unique_ptr<Node> node)
{
    nodes_.emplace_back(std::move(node));
    return nodes_.back().get();
}

}
