////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "instancing_sample.h"

#include <cmath>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include <core/camera.h>
#include <core/error_handling.h>
#include <core/matrix4.h>
#include <core/quaternion.h>
#include <core/root.h>
#include <core/transform.h>
#include <core/vector3.h>
#include <graphics/animation/animation_controller.h>
#include <graphics/cube_map.h>
#include <graphics/instanced_entity.h>
#include <graphics/lights/point_light.h>
#include <graphics/mesh_manager.h>
#include <graphics/render_graph/arithmetic_node.h>
#include <graphics/render_graph/blur_node.h>
#include <graphics/render_graph/combine_node.h>
#include <graphics/render_graph/component_node.h>
#include <graphics/render_graph/composite_node.h>
#include <graphics/render_graph/invert_node.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/render_node.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_graph/value_node.h>
#include <graphics/render_graph/vertex_node.h>
#include <graphics/render_target.h>
#include <graphics/sampler.h>
#include <graphics/scene.h>
#include <graphics/single_entity.h>
#include <graphics/texture_manager.h>
#include <graphics/window.h>
#include <physics/physics_manager.h>
#include <physics/rigid_body.h>

using namespace std::literals::chrono_literals;

namespace
{

/**
 * Helper function to update camera based on user input.
 *
 * @param camera
 *   Camera to update.
 *
 * @param key_map
 *   Map of user pressed keys.
 */
void update_camera(iris::Camera &camera, const std::map<iris::Key, iris::KeyState> &key_map)
{
    static auto speed = 2.0f;
    iris::Vector3 velocity;

    if (key_map.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        velocity += camera.direction() * speed;
    }

    if (key_map.at(iris::Key::S) == iris::KeyState::DOWN)
    {
        velocity -= camera.direction() * speed;
    }

    if (key_map.at(iris::Key::A) == iris::KeyState::DOWN)
    {
        velocity -= camera.right() * speed;
    }

    if (key_map.at(iris::Key::D) == iris::KeyState::DOWN)
    {
        velocity += camera.right() * speed;
    }

    if (key_map.at(iris::Key::Q) == iris::KeyState::DOWN)
    {
        velocity += camera.right().cross(camera.direction()) * speed;
    }

    if (key_map.at(iris::Key::E) == iris::KeyState::DOWN)
    {
        velocity -= camera.right().cross(camera.direction()) * speed;
    }

    camera.translate(velocity);
}

}

InstancingSample::InstancingSample(iris::Window *window, iris::RenderTarget *target)
    : window_(window)
    , ps_(iris::Root::physics_manager().create_physics_system())
    , target_(target)
    , scene_()
    , light_(nullptr)
    , camera_(iris::CameraType::PERSPECTIVE, window_->width(), window_->height(), 10000u)
    , key_map_()
    , sky_box_(nullptr)
{
    auto &mesh_manager = iris::Root::mesh_manager();

    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    camera_.set_position(camera_.position() + iris::Vector3{0.0f, 50.0f, 0.0f});

    scene_.set_ambient_light({0.1f, 0.1f, 0.1f, 1.0f});
    ps_->enable_debug_draw(&scene_);

    const auto *ground_sampler = iris::Root::texture_manager().create(iris::SamplerDescriptor{.uses_mips = true});

    auto *ground_rg = scene_.create_render_graph();
    // ground_rg->render_node()->set_colour_input(ground_rg->create<iris::CombineNode>(
    //    ground_rg->create<iris::VertexNode>(iris::VertexDataType::NORMAL, ".x"),
    //    ground_rg->create<iris::VertexNode>(iris::VertexDataType::NORMAL, ".y"),
    //    ground_rg->create<iris::VertexNode>(iris::VertexDataType::NORMAL, ".z"),
    //    ground_rg->create<iris::ValueNode<float>>(1.0f)));

    ground_rg->render_node()->set_colour_input(ground_rg->create<iris::ArithmeticNode>(
        ground_rg->create<iris::ArithmeticNode>(
            ground_rg->create<iris::ArithmeticNode>(
                ground_rg->create<iris::ValueNode<iris::Colour>>(iris::Colour(iris::Colour{1.0f, 1.0f, 1.0f, 1.0f})),
                ground_rg->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".yyyy"),
                iris::ArithmeticOperator::SUBTRACT),
            ground_rg->create<iris::TextureNode>("grass.jpg", iris::TextureUsage::IMAGE, ground_sampler),
            iris::ArithmeticOperator::MULTIPLY),
        ground_rg->create<iris::ArithmeticNode>(
            ground_rg->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".yyyy"),
            ground_rg->create<iris::TextureNode>("ground.jpg", iris::TextureUsage::IMAGE, ground_sampler),
            iris::ArithmeticOperator::MULTIPLY),
        iris::ArithmeticOperator::ADD));

    const auto *height_map = iris::Root::texture_manager().load("heightmap.png", iris::TextureUsage::DATA);
    const auto *height_map_physics =
        iris::Root::texture_manager().load("heightmap_physics.png", iris::TextureUsage::DATA);

    auto *ground = scene_.create_entity<iris::SingleEntity>(
        ground_rg, mesh_manager.heightmap({}, height_map), iris::Transform{{}, {}, {1000u, 100u, 1000u}});

    auto *ground_body = ps_->create_rigid_body(
        {0.0f, 50.0f, 0.0f},
        ps_->create_heightmap_collision_shape(height_map_physics, {10.0f}),
        iris::RigidBodyType::STATIC);

    std::random_device rd{};
    std::mt19937 engine{rd()};
    std::uniform_real_distribution<float> dist(-490.0f, 490.0f);

    std::vector<iris::Transform> tree_transforms{};

    for (auto i = 0u; i < 1000u; ++i)
    {
        iris::Vector3 position = {dist(engine), 1000.0f, dist(engine)};

        auto results = ps_->ray_cast(position, {0.0f, -1.0f, 0.0f}, {});
        if (!results.empty())
        {
            position.y = results.front().position.y;
        }

        tree_transforms.emplace_back(
            position, iris::Quaternion{{1.0f, 0.0f, 0.0f}, -3.14159254f / 2.0f}, iris::Vector3{2.0f});
    }

    auto *tree_rg = scene_.create_render_graph();
    tree_rg->render_node()->set_colour_input(
        tree_rg->create<iris::ValueNode<iris::Colour>>(iris::Colour{1.0f, 0.0f, 0.0f, 1.0f}));

    scene_.create_entity<iris::InstancedEntity>(tree_rg, mesh_manager.load_mesh("Tree_1.fbx"), tree_transforms);

    // for (const auto &transform : tree_transforms)
    //{
    //    scene_.create_entity<iris::SingleEntity>(tree_rg, mesh_manager.load_mesh("Tree_1.fbx"), transform);
    //}

    sky_box_ = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    light_ =
        scene_.create_light<iris::PointLight>(iris::Vector3{-1.0f, -1.0f, 0.0f}, iris::Colour{400.0f, 400.0f, 400.0f});

    // debug_mesh_ = mesh_manager.unique_cube({});

    // auto *debug_draw = scene_.create_entity<iris::SingleEntity>(nullptr, debug_mesh_.get(), iris::Vector3{},
    // iris::PrimitiveType::LINES); ps_->enable_debug_draw(debug_draw);
}

void InstancingSample::fixed_update()
{
    update_camera(camera_, key_map_);

    light_->set_position(camera_.position() + iris::Vector3{0.0f, 5.0f, 0.0f});

    ps_->step(16ms);
}

void InstancingSample::variable_update()
{
}

void InstancingSample::handle_input(const iris::Event &event)
{
    if (event.is_key())
    {
        const auto keyboard = event.key();
        key_map_[keyboard.key] = keyboard.state;
    }
    else if (event.is_mouse())
    {
        static const auto sensitivity = 0.0025f;
        const auto mouse = event.mouse();

        camera_.adjust_yaw(mouse.delta_x * sensitivity);
        camera_.adjust_pitch(-mouse.delta_y * sensitivity);
    }
}

std::vector<iris::RenderPass> InstancingSample::render_passes()
{
    return {{.scene = &scene_, .camera = &camera_, .colour_target = target_, .sky_box = sky_box_}};
}

std::string InstancingSample::title() const
{
    return "Instancing";
}
