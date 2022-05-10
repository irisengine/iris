////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <deque>
#include <memory>
#include <vector>

#include "graphics/material.h"
#include "graphics/render_pass.h"
#include "graphics/render_queue_builder.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"

#include "fakes/fake_material.h"
#include "fakes/fake_mesh.h"
#include "fakes/fake_render_target.h"

#include <gtest/gtest.h>

class RenderQueueBuilderFixture : public ::testing::Test
{
  public:
    RenderQueueBuilderFixture()
        : builder_(nullptr)
        , materials_()
        , render_targets_()
    {
        builder_ = std::make_unique<iris::RenderQueueBuilder>(
            800u,
            800u,
            [this](auto *, auto *, const auto *, auto, auto, auto) {
                materials_.emplace_back(std::make_unique<FakeMaterial>());
                return materials_.back().get();
            },
            [this](auto, auto) {
                render_targets_.emplace_back(std::make_unique<FakeRenderTarget>());
                return render_targets_.back().get();
            },
            [this](auto, auto) {
                render_targets_.emplace_back(std::make_unique<FakeRenderTarget>());
                return render_targets_.back().get();
            });
    }

  protected:
    std::unique_ptr<iris::RenderQueueBuilder> builder_;
    std::vector<std::unique_ptr<FakeMaterial>> materials_;
    std::vector<std::unique_ptr<FakeRenderTarget>> render_targets_;
    std::vector<iris::Scene> scenes_;
    std::deque<iris::RenderPass> passes_;

    std::deque<iris::RenderPass> create_complex_scene()
    {
        FakeMesh mesh{};

        render_targets_.emplace_back(std::make_unique<FakeRenderTarget>());

        scenes_.emplace_back(iris::Scene{});
        scenes_.emplace_back(iris::Scene{});
        auto &scene1 = scenes_[0];
        auto &scene2 = scenes_[1];

        scene1.create_light<iris::DirectionalLight>(iris::Vector3{}, true);
        scene1.create_entity<iris::SingleEntity>(nullptr, &mesh, iris::Transform{});
        scene2.create_entity<iris::SingleEntity>(nullptr, &mesh, iris::Transform{});

        passes_.push_back({.scene = std::addressof(scenes_[0]), .colour_target = render_targets_.back().get()});
        passes_.push_back({.scene = std::addressof(scenes_[1])});

        return passes_;
    }
};

TEST_F(RenderQueueBuilderFixture, empty_passes)
{
    std::deque<iris::RenderPass> passes{};
    std::vector<iris::RenderCommand> expected(1u);
    expected[0u].set_type(iris::RenderCommandType::PRESENT);

    const auto queue = builder_->build(passes);

    ASSERT_EQ(queue, expected);
}

TEST_F(RenderQueueBuilderFixture, complex_scene)
{
    auto passes = create_complex_scene();

    const auto queue = builder_->build(passes);

    std::vector<iris::RenderCommand> expected{
        {iris::RenderCommandType::PASS_START, std::addressof(passes[0]), nullptr, nullptr, nullptr, nullptr},
        {iris::RenderCommandType::DRAW,
         std::addressof(passes[0]),
         materials_[0].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->ambient_light.get()},
        {iris::RenderCommandType::PASS_END,
         std::addressof(passes[0]),
         materials_[0].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->ambient_light.get()},
        {iris::RenderCommandType::PASS_START,
         std::addressof(passes[1]),
         materials_[0].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->ambient_light.get()},
        {iris::RenderCommandType::DRAW,
         std::addressof(passes[1]),
         materials_[1].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->ambient_light.get()},
        {iris::RenderCommandType::DRAW,
         std::addressof(passes[1]),
         materials_[2].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->directional_lights[0].get()},
        {iris::RenderCommandType::PASS_END,
         std::addressof(passes[1]),
         materials_[2].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->directional_lights[0].get()},
        {iris::RenderCommandType::PASS_START,
         std::addressof(passes[2]),
         materials_[2].get(),
         std::get<1>(scenes_[0].entities()[0]).get(),
         nullptr,
         scenes_[0].lighting_rig()->directional_lights[0].get()},
        {iris::RenderCommandType::DRAW,
         std::addressof(passes[2]),
         materials_[3].get(),
         std::get<1>(scenes_[1].entities()[0]).get(),
         nullptr,
         scenes_[1].lighting_rig()->ambient_light.get()},
        {iris::RenderCommandType::PASS_END,
         std::addressof(passes[2]),
         materials_[3].get(),
         std::get<1>(scenes_[1].entities()[0]).get(),
         nullptr,
         scenes_[1].lighting_rig()->ambient_light.get()},
        {iris::RenderCommandType::PRESENT,
         std::addressof(passes[2]),
         materials_[3].get(),
         std::get<1>(scenes_[1].entities()[0]).get(),
         nullptr,
         scenes_[1].lighting_rig()->ambient_light.get()}};

    ASSERT_EQ(materials_.size(), 4u);
    ASSERT_EQ(render_targets_.size(), 2u);
    ASSERT_EQ(queue, expected);
}
