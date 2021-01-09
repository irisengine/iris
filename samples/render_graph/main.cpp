#define _USE_MATH_DEFINES
#include <cmath>
#include <map>

#include "core/camera.h"
#include "core/looper.h"
#include "core/root.h"
#include "core/transform.h"
#include "graphics/material.h"
#include "graphics/mesh_factory.h"
#include "graphics/pipeline.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/blur_node.h"
#include "graphics/render_graph/colour_node.h"
#include "graphics/render_graph/combine_node.h"
#include "graphics/render_graph/compiler.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/composite_node.h"
#include "graphics/render_graph/invert_node.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/render_node.h"
#include "graphics/render_graph/sin_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_graph/vertex_position_node.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"
#include "graphics/texture_factory.h"
#include "log/log.h"
#include "platform/keyboard_event.h"
#include "platform/resource_loader.h"
#include "platform/start.h"
#include "platform/window.h"

using namespace std::chrono_literals;

void go(int, char **)
{
    LOG_DEBUG("cube_sample", "hello world");

    iris::ResourceLoader::instance().set_root_directory("assets");

    std::map<iris::Key, iris::KeyState> key_map{
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    const auto &window = iris::Root::window();
    iris::Camera camera{iris::CameraType::PERSPECTIVE, 800.0f, 800.0f};
    iris::Camera screen_camera{iris::CameraType::ORTHOGRAPHIC, 800.0f, 800.0f};

    iris::RenderTarget sphere1_rt{800, 800};
    iris::RenderTarget sphere2_rt{800, 800};

    iris::RenderGraph graph1{};

    graph1.render_node()->set_colour_input(graph1.create<iris::InvertNode>(
        graph1.create<iris::TextureNode>("brickwall.jpg")));
    graph1.render_node()->set_normal_input(
        graph1.create<iris::TextureNode>("brickwall_normal.jpg"));

    auto [mesh1, skl1] = iris::mesh_factory::load("sphere.fbx");

    auto scene1 = std::make_unique<iris::Scene>();
    auto *sphere1 = scene1->create_entity(
        std::move(graph1),
        std::move(mesh1),
        iris::Vector3{-20.0f, 0.0f, 0.0f},
        iris::Quaternion({1.0f, 0.0f, 0.0f}, M_PI_2),
        iris::Vector3{10.0f},
        skl1);
    auto *light1 = scene1->create_light(iris::Vector3{-1.0f, -1.0f, 0.0f});

    auto [mesh2, skl2] = iris::mesh_factory::load("sphere.fbx");

    iris::RenderGraph graph2{};
    graph2.render_node()->set_colour_input(
        graph2.create<iris::TextureNode>("brickwall.jpg"));

    auto scene2 = std::make_unique<iris::Scene>();
    auto *sphere2 = scene2->create_entity(
        std::move(graph2),
        std::move(mesh2),
        iris::Vector3{20.0f, 0.0f, 0.0f},
        iris::Quaternion({1.0f, 0.0f, 0.0f}, M_PI_2),
        iris::Vector3{10.0f},
        skl2);
    auto *light2 = scene2->create_light(iris::Vector3{-1.0f, -1.0f, 0.0f});

    iris::RenderGraph graph3{};

    graph3.render_node()->set_colour_input(graph3.create<iris::CompositeNode>(
        graph3.create<iris::TextureNode>(sphere1_rt.colour_texture()),
        graph3.create<iris::BlurNode>(
            graph3.create<iris::TextureNode>(sphere2_rt.colour_texture())),
        graph3.create<iris::TextureNode>(sphere1_rt.depth_texture()),
        graph3.create<iris::TextureNode>(sphere2_rt.depth_texture())));

    auto scene3 = std::make_unique<iris::Scene>();
    scene3->create_entity(
        std::move(graph3),
        iris::mesh_factory::sprite({}),
        iris::Vector3{},
        iris::Vector3{800.0f, 800.0f, 1.0f});

    iris::Pipeline pipeline{};

    pipeline.add_stage(std::move(scene1), camera, sphere1_rt);
    pipeline.add_stage(std::move(scene2), camera, sphere2_rt);
    pipeline.add_stage(std::move(scene3), screen_camera);

    iris::Transform light_transform{light1->direction(), {}, {1.0f}};

    iris::Looper looper{
        0ms,
        33ms,
        [](auto, auto) { return true; },
        [&](auto, auto) {
            if (auto evt = iris::Root::instance().window().pump_event(); evt)
            {
                if (evt->is_key(iris::Key::ESCAPE))
                {
                    return false;
                }
                else if (evt->is_key())
                {
                    const auto keyboard = evt->key();
                    key_map[keyboard.key] = keyboard.state;
                }
                else if (evt->is_mouse())
                {
                    static const auto sensitivity = 0.0025f;
                    const auto mouse = evt->mouse();

                    camera.adjust_yaw(mouse.delta_x * sensitivity);
                    camera.adjust_pitch(-mouse.delta_y * sensitivity);
                }
            }

            static auto speed = 2.0f;
            iris::Vector3 velocity;

            if (key_map[iris::Key::W] == iris::KeyState::DOWN)
            {
                velocity += camera.direction() * speed;
            }

            if (key_map[iris::Key::S] == iris::KeyState::DOWN)
            {
                velocity -= camera.direction() * speed;
            }

            if (key_map[iris::Key::A] == iris::KeyState::DOWN)
            {
                velocity -= camera.right() * speed;
            }

            if (key_map[iris::Key::D] == iris::KeyState::DOWN)
            {
                velocity += camera.right() * speed;
            }

            if (key_map[iris::Key::Q] == iris::KeyState::DOWN)
            {
                velocity += camera.right().cross(camera.direction()) * speed;
            }

            if (key_map[iris::Key::E] == iris::KeyState::DOWN)
            {
                velocity -= camera.right().cross(camera.direction()) * speed;
            }

            camera.translate(velocity);

            light_transform.set_matrix(
                iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) *
                light_transform.matrix());
            light1->set_direction(light_transform.translation());
            light2->set_direction(light_transform.translation());

            window.render(pipeline);

            return true;
        }};

    looper.run();
    LOG_ERROR("cube_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
