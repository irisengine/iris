////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "core/vector3.h"
#include "graphics/render_command.h"
#include "graphics/render_command_type.h"
#include "graphics/render_pass.h"
#include "graphics/single_entity.h"

#include "fakes/fake_light.h"
#include "fakes/fake_material.h"
#include "fakes/fake_render_target.h"

TEST(render_command_tests, default_ctor)
{
    iris::RenderCommand cmd{};

    ASSERT_EQ(cmd.type(), iris::RenderCommandType::PASS_START);
    ASSERT_EQ(cmd.render_pass(), nullptr);
    ASSERT_EQ(cmd.material(), nullptr);
    ASSERT_EQ(cmd.render_entity(), nullptr);
    ASSERT_EQ(cmd.shadow_map(), nullptr);
    ASSERT_EQ(cmd.light(), nullptr);
}

TEST(render_command_tests, ctor)
{
    const auto type = iris::RenderCommandType::DRAW;
    const iris::RenderPass render_pass{nullptr, nullptr, nullptr};
    const FakeMaterial material{};
    const iris::SingleEntity render_entity{nullptr, iris::Vector3{}};
    const FakeRenderTarget shadow_map{};
    const FakeLight light{};

    const iris::RenderCommand cmd{type, &render_pass, &material, &render_entity, &shadow_map, &light};

    ASSERT_EQ(cmd.type(), type);
    ASSERT_EQ(cmd.render_pass(), &render_pass);
    ASSERT_EQ(cmd.material(), &material);
    ASSERT_EQ(cmd.render_entity(), &render_entity);
    ASSERT_EQ(cmd.shadow_map(), &shadow_map);
    ASSERT_EQ(cmd.light(), &light);
}

TEST(render_command_tests, get_set_type)
{
    iris::RenderCommand cmd{};
    const auto new_value = iris::RenderCommandType::DRAW;

    cmd.set_type(new_value);

    ASSERT_EQ(cmd.type(), new_value);
}

TEST(render_command_tests, get_set_render_pass)
{
    iris::RenderCommand cmd{};
    const iris::RenderPass new_value{nullptr, nullptr, nullptr};

    cmd.set_render_pass(&new_value);

    ASSERT_EQ(cmd.render_pass(), &new_value);
}

TEST(render_command_tests, get_set_material)
{
    iris::RenderCommand cmd{};
    const FakeMaterial new_value{};

    cmd.set_material(&new_value);

    ASSERT_EQ(cmd.material(), &new_value);
}

TEST(render_command_tests, get_set_render_entity)
{
    iris::RenderCommand cmd{};
    const iris::SingleEntity new_value{nullptr, iris::Vector3{}};

    cmd.set_render_entity(&new_value);

    ASSERT_EQ(cmd.render_entity(), &new_value);
}

TEST(render_command_tests, get_set_shadow_map)
{
    iris::RenderCommand cmd{};
    const FakeRenderTarget new_value{};

    cmd.set_shadow_map(&new_value);

    ASSERT_EQ(cmd.shadow_map(), &new_value);
}

TEST(render_command_tests, get_set_light)
{
    iris::RenderCommand cmd{};
    const FakeLight new_value{};

    cmd.set_light(&new_value);

    ASSERT_EQ(cmd.light(), &new_value);
}

TEST(render_command_tests, equality)
{
    const auto type = iris::RenderCommandType::DRAW;
    const iris::RenderPass render_pass{nullptr, nullptr, nullptr};
    const FakeMaterial material{};
    const iris::SingleEntity render_entity{nullptr, iris::Vector3{}};
    const FakeRenderTarget shadow_map{};
    const FakeLight light{};

    const iris::RenderCommand cmd1{type, &render_pass, &material, &render_entity, &shadow_map, &light};

    const iris::RenderCommand cmd2{type, &render_pass, &material, &render_entity, &shadow_map, &light};

    ASSERT_EQ(cmd1, cmd2);
}

TEST(render_command_tests, inequality)
{
    const auto type = iris::RenderCommandType::DRAW;
    const iris::RenderPass render_pass{nullptr, nullptr, nullptr};
    const FakeMaterial material{};
    const iris::SingleEntity render_entity{nullptr, iris::Vector3{}};
    const FakeRenderTarget shadow_map{};
    const FakeLight light{};

    const iris::RenderCommand cmd1{type, &render_pass, &material, &render_entity, &shadow_map, &light};

    const iris::RenderCommand cmd2{type, &render_pass, nullptr, &render_entity, &shadow_map, &light};

    ASSERT_NE(cmd1, cmd2);
}
