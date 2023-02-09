////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/render_graph.h"

#include <memory>

#include "graphics/render_graph/node.h"
#include "graphics/render_graph/property.h"
#include "graphics/render_graph/render_node.h"

namespace iris
{

RenderGraph::RenderGraph(std::span<std::byte> property_buffer)
    : nodes_()
    , variables_()
    , properties_()
    , property_buffer_(property_buffer)
    , offset_(0u)
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

std::vector<RenderGraphVariable> RenderGraph::variables() const
{
    return variables_;
}

const std::deque<Property> &RenderGraph::properties() const
{
    return properties_;
}

std::span<std::byte> RenderGraph::property_buffer() const
{
    return property_buffer_;
}

}
