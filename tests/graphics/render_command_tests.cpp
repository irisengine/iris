////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "core/vector3.h"
#include "graphics/mesh_loader.h"
#include "graphics/render_command.h"
#include "graphics/render_command_type.h"
#include "graphics/render_pass.h"
#include "graphics/render_pipeline.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"

#include "fakes/fake_light.h"
#include "fakes/fake_material.h"
#include "fakes/fake_mesh.h"
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
    iris::RenderPipeline pipeline{600u, 600u};
    FakeMesh mesh{};
    const auto type = iris::RenderCommandType::DRAW;
    const FakeMaterial material{};
    const iris::SingleEntity render_entity{&mesh, iris::Vector3{}};
    const FakeRenderTarget shadow_map{};
    const FakeLight light{};
    auto *scene = pipeline.create_scene();
    const auto *render_pass = pipeline.create_render_pass(scene);

    const iris::RenderCommand cmd{type, render_pass, &material, &render_entity, &shadow_map, &light};

    ASSERT_EQ(cmd.type(), type);
    ASSERT_EQ(cmd.render_pass(), render_pass);
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
    iris::RenderPipeline pipeline{600u, 600u};
    auto *scene = pipeline.create_scene();
    const auto *render_pass = pipeline.create_render_pass(scene);
    iris::RenderCommand cmd{};

    cmd.set_render_pass(render_pass);

    ASSERT_EQ(cmd.render_pass(), render_pass);
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
    FakeMesh mesh{};
    iris::RenderCommand cmd{};
    const iris::SingleEntity new_value{&mesh, iris::Vector3{}};

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
