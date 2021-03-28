#include "render_graph_sample.h"

#include <cmath>
#include <memory>

#include <core/camera.h>
#include <core/matrix4.h>
#include <core/quaternion.h>
#include <core/transform.h>
#include <core/vector3.h>
#include <core/window.h>
#include <graphics/lights/point_light.h>
#include <graphics/mesh_factory.h>
#include <graphics/pipeline.h>
#include <graphics/render_graph/blur_node.h>
#include <graphics/render_graph/composite_node.h>
#include <graphics/render_graph/invert_node.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/render_node.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_target.h>
#include <graphics/scene.h>

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
void update_camera(
    iris::Camera &camera,
    const std::map<iris::Key, iris::KeyState> &key_map)
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

RenderGraphSample::RenderGraphSample(
    iris::Window *window,
    iris::RenderTarget *target)
    : window_(window)
    , pipeline_()
    , light_transform_()
    , light1_(nullptr)
    , light2_(nullptr)
    , sphere1_rt_(window_->width(), window_->height())
    , sphere2_rt_(window_->width(), window_->height())
    , camera_(
          iris::CameraType::PERSPECTIVE,
          window_->width(),
          window_->height())
    , screen_camera_(
          iris::CameraType::ORTHOGRAPHIC,
          window_->width(),
          window_->height())
    , key_map_()
{
    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    auto scene1 = std::make_unique<iris::Scene>();
    auto *graph1 = scene1->create_render_graph();

    graph1->render_node()->set_colour_input(graph1->create<iris::InvertNode>(
        graph1->create<iris::TextureNode>("brickwall.jpg")));
    graph1->render_node()->set_normal_input(
        graph1->create<iris::TextureNode>("brickwall_normal.jpg"));

    auto [mesh1, skl1] = iris::mesh_factory::load("sphere.fbx");

    scene1->set_ambient_light({0.15f, 0.15f, 0.15f});
    light1_ = scene1->create_light<iris::PointLight>(
        iris::Vector3{0.0f, 100.0f, -100.0f});

    auto *sphere1 = scene1->create_entity(
        graph1,
        std::move(mesh1),
        iris::Transform{
            iris::Vector3{-20.0f, 0.0f, 0.0f},
            iris::Quaternion({1.0f, 0.0f, 0.0f}, 1.57079632679489661923f),
            iris::Vector3{10.0f}},
        skl1);

    auto [mesh2, skl2] = iris::mesh_factory::load("sphere.fbx");

    auto scene2 = std::make_unique<iris::Scene>();
    auto *graph2 = scene2->create_render_graph();

    graph2->render_node()->set_colour_input(
        graph2->create<iris::TextureNode>("brickwall.jpg"));

    scene2->set_ambient_light({0.15f, 0.15f, 0.15f});
    light2_ = scene2->create_light<iris::PointLight>(
        iris::Vector3{0.0f, 100.0f, -100.0f});

    auto *sphere2 = scene2->create_entity(
        graph2,
        std::move(mesh2),
        iris::Transform{
            iris::Vector3{20.0f, 0.0f, 0.0f},
            iris::Quaternion({1.0f, 0.0f, 0.0f}, 1.57079632679489661923f),
            iris::Vector3{10.0f}},
        skl2);

    auto scene3 = std::make_unique<iris::Scene>();
    auto *graph3 = scene3->create_render_graph();

    graph3->render_node()->set_colour_input(graph3->create<iris::CompositeNode>(
        graph3->create<iris::TextureNode>(sphere1_rt_.colour_texture()),
        graph3->create<iris::BlurNode>(
            graph3->create<iris::TextureNode>(sphere2_rt_.colour_texture())),
        graph3->create<iris::TextureNode>(sphere1_rt_.depth_texture()),
        graph3->create<iris::TextureNode>(sphere2_rt_.depth_texture())));

    scene3->create_entity(
        graph3,
        iris::mesh_factory::sprite({}),
        iris::Transform{
            iris::Vector3{}, {}, iris::Vector3{800.0f, 800.0f, 1.0f}});

    pipeline_.add_stage(std::move(scene1), camera_, &sphere1_rt_);
    pipeline_.add_stage(std::move(scene2), camera_, &sphere2_rt_);
    pipeline_.add_stage(std::move(scene3), screen_camera_, target);

    light_transform_ = iris::Transform{light1_->position(), {}, {1.0f}};
}

void RenderGraphSample::fixed_update()
{
}

void RenderGraphSample::variable_update()
{
    update_camera(camera_, key_map_);

    light_transform_.set_matrix(
        iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) *
        light_transform_.matrix());
    light1_->set_position(light_transform_.translation());
    light2_->set_position(light_transform_.translation());

    window_->render(pipeline_);
}

void RenderGraphSample::handle_input(const iris::Event &event)
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

std::string RenderGraphSample::title() const
{
    return "Render graph";
}
