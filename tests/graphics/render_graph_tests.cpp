////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <functional>

#include <gtest/gtest.h>

#include "fakes/fake_texture.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"

TEST(render_graph_tests, empty_graph_hash_the_same)
{
    iris::RenderGraph rg1{};
    iris::RenderGraph rg2{};

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(&rg1), std::hash<iris::RenderGraph *>{}(&rg2));
}

TEST(render_graph_tests, simple_value_node_hash_the_same)
{
    iris::RenderGraph rg1{};
    rg1.render_node()->set_colour_input(rg1.create<iris::ValueNode<float>>(1.2f));

    iris::RenderGraph rg2{};
    rg2.render_node()->set_colour_input(rg2.create<iris::ValueNode<float>>(1.2f));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(&rg1), std::hash<iris::RenderGraph *>{}(&rg2));
}

TEST(render_graph_tests, simple_value_node_hash_different)
{
    iris::RenderGraph rg1{};
    rg1.render_node()->set_colour_input(rg1.create<iris::ValueNode<float>>(1.2f));

    iris::RenderGraph rg2{};
    rg2.render_node()->set_colour_input(rg2.create<iris::ValueNode<float>>(1.3f));

    ASSERT_NE(std::hash<iris::RenderGraph *>{}(&rg1), std::hash<iris::RenderGraph *>{}(&rg2));
}

TEST(render_graph_tests, texture_node_single_texture_hash_the_same)
{
    FakeTexture texture{};

    iris::RenderGraph rg1{};
    rg1.render_node()->set_colour_input(rg1.create<iris::TextureNode>(&texture));

    iris::RenderGraph rg2{};
    rg2.render_node()->set_colour_input(rg2.create<iris::TextureNode>(&texture));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(&rg1), std::hash<iris::RenderGraph *>{}(&rg2));
}

TEST(render_graph_tests, texture_node_multiple_texture_hash_the_same)
{
    FakeTexture texture1{};
    FakeTexture texture2{};

    iris::RenderGraph rg1{};
    rg1.render_node()->set_colour_input(rg1.create<iris::TextureNode>(&texture1));

    iris::RenderGraph rg2{};
    rg2.render_node()->set_colour_input(rg2.create<iris::TextureNode>(&texture2));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(&rg1), std::hash<iris::RenderGraph *>{}(&rg2));
}

TEST(render_graph_tests, complex_graph_hash_the_same)
{
    FakeTexture texture1{};
    FakeTexture texture2{};

    iris::RenderGraph rg1{};
    rg1.render_node()->set_colour_input(rg1.create<iris::TextureNode>(&texture1));
    rg1.render_node()->set_specular_amount_input(
        rg1.create<iris::ComponentNode>(rg1.create<iris::TextureNode>(&texture2), "r"));

    iris::RenderGraph rg2{};
    rg2.render_node()->set_colour_input(rg1.create<iris::TextureNode>(&texture1));
    rg2.render_node()->set_specular_amount_input(
        rg2.create<iris::ComponentNode>(rg2.create<iris::TextureNode>(&texture2), "r"));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(&rg1), std::hash<iris::RenderGraph *>{}(&rg2));
}
