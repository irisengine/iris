////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "render_graph_sample.h"

#include <cmath>
#include <memory>

#include <core/camera.h>
#include <core/error_handling.h>
#include <core/matrix4.h>
#include <core/quaternion.h>
#include <core/root.h>
#include <core/transform.h>
#include <core/vector3.h>
#include <graphics/lights/point_light.h>
#include <graphics/mesh_manager.h>
#include <graphics/render_graph/blur_node.h>
#include <graphics/render_graph/combine_node.h>
#include <graphics/render_graph/composite_node.h>
#include <graphics/render_graph/invert_node.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/render_node.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_graph/value_node.h>
#include <graphics/render_graph/vertex_node.h>
#include <graphics/render_pipeline.h>
#include <graphics/render_target.h>
#include <graphics/render_target_manager.h>
#include <graphics/scene.h>
#include <graphics/single_entity.h>
#include <graphics/texture_manager.h>
#include <graphics/window.h>

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

RenderGraphSample::RenderGraphSample(iris::Window *window, iris::RenderPipeline &render_pipeline)
    : light_transform_()
    , light1_(nullptr)
    , light2_(nullptr)
    , camera_(iris::CameraType::PERSPECTIVE, window->width(), window->height())
    , screen_camera_(iris::CameraType::ORTHOGRAPHIC, window->width(), window->height())
    , key_map_()
    , sky_box_(nullptr)
{
    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    auto &mesh_manager = iris::Root::mesh_manager();
    auto &rtm = iris::Root::render_target_manager();

    auto meshes = mesh_manager.load_mesh("sphere.fbx");
    iris::ensure(meshes.mesh_data.size() == 1u, "expecting single mesh");

    auto *scene1 = render_pipeline.create_scene();
    auto *graph1 = render_pipeline.create_render_graph();
    graph1->render_node()->set_colour_input(
        graph1->create<iris::InvertNode>(graph1->create<iris::TextureNode>("brickwall.jpg")));
    graph1->render_node()->set_normal_input(
        graph1->create<iris::TextureNode>("brickwall_normal.jpg", iris::TextureUsage::DATA));

    scene1->set_ambient_light({0.15f, 0.15f, 0.15f});
    light1_ = scene1->create_light<iris::PointLight>(
        iris::Vector3{0.0f, 50.0f, -50.0f}, iris::Colour{200.0f, 200.0f, 200.0f});

    auto *mesh = meshes.mesh_data.front().mesh;

    auto *sphere1_rt = rtm.create();
    auto *sphere1 = scene1->create_entity<iris::SingleEntity>(
        graph1,
        mesh,
        iris::Transform{
            iris::Vector3{-20.0f, 0.0f, 0.0f},
            iris::Quaternion({1.0f, 0.0f, 0.0f}, 1.57079632679489661923f),
            iris::Vector3{10.0f}});

    auto *scene2 = render_pipeline.create_scene();
    auto *graph2 = render_pipeline.create_render_graph();
    graph2->render_node()->set_colour_input(graph2->create<iris::TextureNode>("brickwall.jpg"));
    scene2->set_ambient_light({0.15f, 0.15f, 0.15f});
    light2_ = scene2->create_light<iris::PointLight>(
        iris::Vector3{0.0f, 50.0f, -50.0f}, iris::Colour{100.0f, 100.0f, 100.0f});

    auto *sphere2_rt = rtm.create();
    auto *sphere2 = scene2->create_entity<iris::SingleEntity>(
        graph2,
        mesh,
        iris::Transform{
            iris::Vector3{20.0f, 0.0f, 0.0f},
            iris::Quaternion({1.0f, 0.0f, 0.0f}, 1.57079632679489661923f),
            iris::Vector3{10.0f}});

    auto *scene3 = render_pipeline.create_scene();
    auto *graph3 = render_pipeline.create_render_graph();
    graph3->render_node()->set_colour_input(graph3->create<iris::CompositeNode>(
        graph3->create<iris::BlurNode>(graph3->create<iris::TextureNode>(sphere2_rt->colour_texture())),
        graph3->create<iris::TextureNode>(sphere1_rt->colour_texture()),
        graph3->create<iris::TextureNode>(sphere2_rt->depth_texture()),
        graph3->create<iris::TextureNode>(sphere1_rt->depth_texture())));

    scene3->create_entity<iris::SingleEntity>(
        graph3,
        mesh_manager.sprite({}, 1.0f),
        iris::Transform{iris::Vector3{}, {}, iris::Vector3{1920.0f, 1080.0f, 1.0f}});

    auto *graph4 = render_pipeline.create_render_graph();
    graph4->render_node()->set_colour_input(graph4->create<iris::CombineNode>(
        graph4->create<iris::VertexNode>(iris::VertexDataType::NORMAL, ".r"),
        graph4->create<iris::VertexNode>(iris::VertexDataType::NORMAL, ".g"),
        graph4->create<iris::VertexNode>(iris::VertexDataType::NORMAL, ".b"),
        graph4->create<iris::ValueNode<float>>(1.0f)));

    auto *sphere3 = scene1->create_entity<iris::SingleEntity>(
        graph4, mesh, iris::Transform{iris::Vector3{60.0f, 0.0f, 0.0f}, iris::Quaternion{}, iris::Vector3{10.0f}});

    auto *graph5 = render_pipeline.create_render_graph();
    graph5->render_node()->set_colour_input(
        graph5->create<iris::ValueNode<iris::Colour>>(iris::Colour{100.0f, 0.0f, 0.0f, 1.0f}));

    auto *sphere4 = scene1->create_entity<iris::SingleEntity>(
        graph5,
        mesh,
        iris::Transform{
            iris::Vector3{-60.0f, 0.0f, 0.0f},
            iris::Quaternion({1.0f, 0.0f, 0.0f}, 1.57079632679489661923f),
            iris::Vector3{10.0f}});

    light_transform_ = iris::Transform{light1_->position(), {}, {1.0f}};

    sky_box_ = iris::Root::texture_manager().load(
        "space/right.png", "space/left.png", "space/top.png", "space/bottom.png", "space/back.png", "space/front.png");

    auto *pass1 = render_pipeline.create_render_pass(scene1);
    pass1->colour_target = sphere1_rt;
    pass1->camera = &camera_;
    pass1->sky_box = sky_box_;

    auto *pass2 = render_pipeline.create_render_pass(scene2);
    pass2->colour_target = sphere2_rt;
    pass2->camera = &camera_;
    pass2->sky_box = sky_box_;

    auto *pass3 = render_pipeline.create_render_pass(scene3);
    pass3->colour_target = iris::Root::render_target_manager().create();
    pass3->camera = &screen_camera_;
    pass3->sky_box = sky_box_;

    render_target_ = pass3->colour_target;
}

void RenderGraphSample::fixed_update()
{
    update_camera(camera_, key_map_);

    light_transform_.set_matrix(
        iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) * light_transform_.matrix());
    light1_->set_position(light_transform_.translation());
    light2_->set_position(light_transform_.translation());
}

void RenderGraphSample::variable_update()
{
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

const iris::RenderTarget *RenderGraphSample::target() const
{
    return render_target_;
}
