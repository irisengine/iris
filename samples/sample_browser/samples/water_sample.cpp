////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "water_sample.h"

#include <cmath>
#include <map>
#include <memory>
#include <numbers>
#include <string>
#include <vector>

#include <core/camera.h>
#include <core/error_handling.h>
#include <core/matrix4.h>
#include <core/quaternion.h>
#include <core/root.h>
#include <core/transform.h>
#include <core/vector3.h>
#include <graphics/cube_map.h>
#include <graphics/lights/point_light.h>
#include <graphics/mesh_manager.h>
#include <graphics/post_processing_description.h>
#include <graphics/render_graph/arithmetic_node.h>
#include <graphics/render_graph/blur_node.h>
#include <graphics/render_graph/colour_node.h>
#include <graphics/render_graph/combine_node.h>
#include <graphics/render_graph/composite_node.h>
#include <graphics/render_graph/invert_node.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/render_node.h>
#include <graphics/render_graph/sin_node.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_graph/time_node.h>
#include <graphics/render_graph/value_node.h>
#include <graphics/render_graph/vertex_node.h>
#include <graphics/render_pipeline.h>
#include <graphics/render_target.h>
#include <graphics/render_target_manager.h>
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

WaterSample::WaterSample(iris::Window *window, iris::RenderPipeline &render_pipeline)
    : light_transform_()
    , light_(nullptr)
    , camera_(iris::CameraType::PERSPECTIVE, window->width(), window->height())
    , key_map_()
    , sky_box_(nullptr)
    , scene_(nullptr)
    , render_target_(nullptr)
{
    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    camera_.set_position(camera_.position() + iris::Vector3{0.0f, 5.0f, 0.0f});

    scene_ = render_pipeline.create_scene();
    scene_->set_ambient_light({0.5f, 0.5f, 0.5f, 1.0f});

    auto &mesh_manager = iris::Root::mesh_manager();

    sky_box_ = iris::Root::texture_manager().create(
        iris::Colour{0.4f, 0.4f, 0.4f}, iris::Colour{0.2f, 0.2f, 0.2f}, 2048u, 2048u);

    static constexpr auto wavelength = 10.0f;
    static constexpr auto amplitude = 0.1f;
    static constexpr auto speed = 0.001f;

    auto *water_graph = render_pipeline.create_render_graph();
    water_graph->render_node()->set_position_input(water_graph->create<iris::CombineNode>(
        water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".x"),
        water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".y"),
        water_graph->create<iris::ArithmeticNode>(
            water_graph->create<iris::ValueNode<float>>(amplitude),
            water_graph->create<iris::SinNode>(water_graph->create<iris::ArithmeticNode>(
                water_graph->create<iris::ValueNode<float>>(wavelength),
                water_graph->create<iris::ArithmeticNode>(
                    water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".x"),
                    water_graph->create<iris::ArithmeticNode>(
                        water_graph->create<iris::ValueNode<float>>(speed),
                        water_graph->create<iris::TimeNode>(),
                        iris::ArithmeticOperator::MULTIPLY),
                    iris::ArithmeticOperator::SUBTRACT),
                iris::ArithmeticOperator::MULTIPLY)),
            iris::ArithmeticOperator::MULTIPLY),

        // water_graph->create<iris::ArithmeticNode>(
        //    water_graph->create<iris::ValueNode<float>>(amplitude),
        //    water_graph->create<iris::SinNode>(water_graph->create<iris::ArithmeticNode>(
        //        water_graph->create<iris::ValueNode<float>>(2.0f * std::numbers::pi_v<float> / wavelength),
        //        water_graph->create<iris::ArithmeticNode>(
        //            water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".y"),
        //            water_graph->create<iris::ArithmeticNode>(
        //                water_graph->create<iris::ValueNode<float>>(speed),
        //                // water_graph->create<iris::TimeNode>(),
        //                water_graph->create<iris::ValueNode<float>>(0.0f),
        //                iris::ArithmeticOperator::MULTIPLY),
        //            iris::ArithmeticOperator::SUBTRACT),
        //        iris::ArithmeticOperator::MULTIPLY)),
        //    iris::ArithmeticOperator::MULTIPLY),

        // water_graph->create<iris::SinNode>(water_graph->create<iris::ArithmeticNode>(
        //    water_graph->create<iris::SinNode>(water_graph->create<iris::ArithmeticNode>(
        //        water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".x"),
        //        water_graph->create<iris::ValueNode<float>>((2.0f * std::numbers::pi_v<float>) / 0.5f),
        //        iris::ArithmeticOperator::MULTIPLY)),
        //    water_graph->create<iris::ValueNode<float>>(0.05f),
        //    iris::ArithmeticOperator::MULTIPLY)),
        // water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".y"),

        water_graph->create<iris::ValueNode<float>>(1.0f)));

    auto *water = scene_->create_entity<iris::SingleEntity>(
        water_graph,
        iris::Root::mesh_manager().plane({0.0f, 0.0f, 1.0f}, 20),
        iris::Transform{{0.0f, 0.0f, 0.0f}, {{1.0f, 0.0f, 0.0f}, -1.5707f}, {100.0f}});
    water->set_wireframe(true);

    light_ =
        scene_->create_light<iris::PointLight>(iris::Vector3{0.0f, 0.5f, -10.0f}, iris::Colour{50.0f, 50.0f, 50.0f});
    light_transform_ = iris::Transform{light_->position(), {}, {1.0f}};

    auto *pass = render_pipeline.create_render_pass(scene_);
    pass->colour_target = iris::Root::render_target_manager().create();
    pass->camera = &camera_;
    pass->sky_box = sky_box_;

    render_target_ = pass->colour_target;
}

void WaterSample::fixed_update()
{
    update_camera(camera_, key_map_);

    light_transform_.set_matrix(
        iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) * light_transform_.matrix());
    light_->set_position(light_transform_.translation());
}

void WaterSample::variable_update()
{
}

void WaterSample::handle_input(const iris::Event &event)
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

std::string WaterSample::title() const
{
    return "Water";
}

const iris::RenderTarget *WaterSample::target() const
{
    return render_target_;
}
