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
#include "graphics/render_pipeline.h"

#include "mocks/mock_material_manager.h"
#include "mocks/mock_mesh_manager.h"
#include "mocks/mock_render_target_manager.h"
#include "mocks/mock_resource_manager.h"

class RenderGraphTest : public ::testing::Test
{
  public:
    RenderGraphTest()
        : resource_manager_()
        , material_manager_()
        , mesh_manager_(resource_manager_)
        , render_target_manager_()
        , pipeline_(material_manager_, mesh_manager_, render_target_manager_, 600u, 600u)
    {
    }

    iris::RenderGraph *create_render_graph()
    {
        return pipeline_.create_render_graph();
    }

  private:
    MockResourceManager resource_manager_;
    MockMaterialManager material_manager_;
    MockMeshManager mesh_manager_;
    MockRenderTargetManager render_target_manager_;
    iris::RenderPipeline pipeline_;
};

TEST_F(RenderGraphTest, empty_graph_hash_the_same)
{
    const auto *rg1 = create_render_graph();
    const auto *rg2 = create_render_graph();

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(rg1), std::hash<iris::RenderGraph *>{}(rg2));
}

TEST_F(RenderGraphTest, simple_value_node_hash_the_same)
{
    auto *rg1 = create_render_graph();
    rg1->render_node()->set_colour_input(rg1->create<iris::ValueNode<float>>(1.2f));

    auto *rg2 = create_render_graph();
    rg2->render_node()->set_colour_input(rg2->create<iris::ValueNode<float>>(1.2f));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(rg1), std::hash<iris::RenderGraph *>{}(rg2));
}

TEST_F(RenderGraphTest, simple_value_node_hash_different)
{
    auto *rg1 = create_render_graph();
    rg1->render_node()->set_colour_input(rg1->create<iris::ValueNode<float>>(1.2f));

    auto *rg2 = create_render_graph();
    rg2->render_node()->set_colour_input(rg2->create<iris::ValueNode<float>>(1.3f));

    ASSERT_NE(std::hash<iris::RenderGraph *>{}(rg1), std::hash<iris::RenderGraph *>{}(rg2));
}

TEST_F(RenderGraphTest, texture_node_single_texture_hash_the_same)
{
    FakeTexture texture{};

    auto *rg1 = create_render_graph();
    rg1->render_node()->set_colour_input(rg1->create<iris::TextureNode>(&texture));

    auto *rg2 = create_render_graph();
    rg2->render_node()->set_colour_input(rg2->create<iris::TextureNode>(&texture));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(rg1), std::hash<iris::RenderGraph *>{}(rg2));
}

TEST_F(RenderGraphTest, texture_node_multiple_texture_hash_different)
{
    FakeTexture texture1{};
    FakeTexture texture2{};

    auto *rg1 = create_render_graph();
    rg1->render_node()->set_colour_input(rg1->create<iris::TextureNode>(&texture1));

    auto *rg2 = create_render_graph();
    rg2->render_node()->set_colour_input(rg2->create<iris::TextureNode>(&texture2));

    ASSERT_NE(std::hash<iris::RenderGraph *>{}(rg1), std::hash<iris::RenderGraph *>{}(rg2));
}

TEST_F(RenderGraphTest, complex_graph_hash_the_same)
{
    FakeTexture texture1{};
    FakeTexture texture2{};

    auto *rg1 = create_render_graph();
    rg1->render_node()->set_colour_input(rg1->create<iris::TextureNode>(&texture1));
    rg1->render_node()->set_specular_amount_input(
        rg1->create<iris::ComponentNode>(rg1->create<iris::TextureNode>(&texture2), "r"));

    auto *rg2 = create_render_graph();
    rg2->render_node()->set_colour_input(rg1->create<iris::TextureNode>(&texture1));
    rg2->render_node()->set_specular_amount_input(
        rg2->create<iris::ComponentNode>(rg2->create<iris::TextureNode>(&texture2), "r"));

    ASSERT_EQ(std::hash<iris::RenderGraph *>{}(rg1), std::hash<iris::RenderGraph *>{}(rg2));
}
