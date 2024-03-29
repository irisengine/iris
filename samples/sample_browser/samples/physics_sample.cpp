////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics_sample.h"

#include <chrono>
#include <cmath>
#include <memory>
#include <sstream>

#include <core/camera.h>
#include <core/context.h>
#include <core/matrix4.h>
#include <core/quaternion.h>
#include <core/transform.h>
#include <core/vector3.h>
#include <graphics/cube_map.h>
#include <graphics/lights/point_light.h>
#include <graphics/mesh_manager.h>
#include <graphics/render_graph/component_node.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/render_node.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_graph/value_node.h>
#include <graphics/render_pipeline.h>
#include <graphics/render_target.h>
#include <graphics/render_target_manager.h>
#include <graphics/sampler.h>
#include <graphics/scene.h>
#include <graphics/single_entity.h>
#include <graphics/texture_manager.h>
#include <graphics/window.h>
#include <log/log.h>
#include <physics/basic_character_controller.h>
#include <physics/physics_manager.h>

using namespace std::chrono_literals;

namespace
{

/**
 * Helper function to update camera based on user input.
 *
 * @param camera
 *   Camera to update.
 *
 * @param character_controller
 *   Player character controller.
 *
 * @param key_map
 *   Map of user pressed keys.
 */
void update_camera(
    iris::Camera &camera,
    iris::CharacterController *character_controller,
    const std::map<iris::Key, iris::KeyState> &key_map)
{
    iris::Vector3 walk_direction{};

    if (key_map.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        walk_direction += camera.direction();
    }

    if (key_map.at(iris::Key::S) == iris::KeyState::DOWN)
    {
        walk_direction -= camera.direction();
    }

    if (key_map.at(iris::Key::A) == iris::KeyState::DOWN)
    {
        walk_direction -= camera.right();
    }

    if (key_map.at(iris::Key::D) == iris::KeyState::DOWN)
    {
        walk_direction += camera.right();
    }

    walk_direction.normalise();
    character_controller->set_movement_direction(walk_direction);

    const auto cam_pos = character_controller->position();

    camera.translate(cam_pos - camera.position() + iris::Vector3{0.0f, 0.5f, 0.0f});
}

}

PhysicsSample::PhysicsSample(iris::Window *window, iris::RenderPipeline &render_pipeline, iris::Context &context)
    : render_target_(nullptr)
    , physics_(context.physics_manager().create_physics_system())
    , light_transform_()
    , light_(nullptr)
    , camera_(iris::CameraType::PERSPECTIVE, window->width(), window->height())
    , key_map_()
    , boxes_()
    , character_controller_(nullptr)
{
    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
        {iris::Key::SPACE, iris::KeyState::UP},
    };

    camera_.set_position(camera_.position() + iris::Vector3{0.0f, 5.0f, 0.0f});

    auto *scene = render_pipeline.create_scene();

    scene->set_ambient_light({0.4f, 0.4f, 0.4f});
    scene->create_light<iris::DirectionalLight>(iris::Vector3{0.0f, -1.0f, -1.0f}, true);
    light_ = scene->create_light<iris::PointLight>(iris::Vector3{0.0f, 1.0f, -10.0f});

    auto *floor_graph = render_pipeline.create_render_graph();
    floor_graph->render_node()->set_specular_amount_input(floor_graph->create<iris::ValueNode<float>>(0.0f));

    auto &mesh_manager = context.mesh_manager();
    auto &texture_manager = context.texture_manager();
    auto &render_target_manager = context.render_target_manager();

    scene->create_entity<iris::SingleEntity>(
        floor_graph,
        mesh_manager.cube({1.0f, 1.0f, 1.0f}),
        iris::Transform{iris::Vector3{0.0f, -50.0f, 0.0f}, {}, iris::Vector3{500.0f, 50.0f, 500.0f}});

    auto width = 10u;
    auto height = 5u;

    const auto *texture = texture_manager.load("crate.png");

    for (auto y = 0u; y < height; ++y)
    {
        for (auto x = 0u; x < width; ++x)
        {
            auto *box_graph = render_pipeline.create_render_graph();
            box_graph->render_node()->set_colour_input(box_graph->create<iris::TextureNode>(texture));

            const iris::Vector3 pos{static_cast<float>(x), static_cast<float>(y + 0.5f), 0.0f};
            static const iris::Vector3 half_size{0.5f, 0.5f, 0.5f};
            auto colour =
                ((y * height) + x + (y % 2)) % 2 == 0 ? iris::Colour{1.0f, 0.0f, 0.0f} : iris::Colour{0.0f, 0.0f, 1.0f};

            boxes_.emplace_back(
                scene->create_entity<iris::SingleEntity>(
                    box_graph, mesh_manager.cube({1.0f, 1.0f, 1.0f}), iris::Transform{pos, {}, half_size}),
                physics_->create_rigid_body(
                    pos, physics_->create_box_collision_shape(half_size), iris::RigidBodyType::NORMAL));

            std::stringstream strm{};
            strm << "box_" << x << "_" << y;
            std::get<0>(boxes_.back())->set_name(strm.str());
        }
    }

    light_transform_ = iris::Transform{light_->position(), {}, {1.0f}};

    character_controller_ =
        physics_->create_character_controller<iris::BasicCharacterController>(physics_, 12.0f, 0.5f, 1.7f, 0.1f);

    physics_->create_rigid_body(
        iris::Vector3{0.0f, -50.0f, 0.0f},
        physics_->create_box_collision_shape({500.0f, 50.0f, 500.0f}),
        iris::RigidBodyType::STATIC);

    const auto *sky_box =
        texture_manager.create(iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    auto *pass = render_pipeline.create_render_pass(scene);
    pass->colour_target = render_target_manager.create();
    pass->camera = &camera_;
    pass->sky_box = sky_box;

    render_target_ = pass->colour_target;

    physics_->enable_debug_draw(scene);
}

void PhysicsSample::fixed_update()
{
    update_camera(camera_, character_controller_, key_map_);

    light_transform_.set_matrix(
        iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) * light_transform_.matrix());
    light_->set_position(light_transform_.translation());

    physics_->step(16ms);
}

void PhysicsSample::variable_update()
{
    // update each entity to have the same position and orientation as the
    // physics simulation
    for (const auto &[g, p] : boxes_)
    {
        g->set_position(p->position());
        g->set_orientation(p->orientation());
    }
}

void PhysicsSample::handle_input(const iris::Event &event)
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

std::string PhysicsSample::title() const
{
    return "Physics";
}

const iris::RenderTarget *PhysicsSample::target() const
{
    return render_target_;
}
