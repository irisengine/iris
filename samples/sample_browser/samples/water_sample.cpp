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
#include <core/context.h>
#include <core/error_handling.h>
#include <core/matrix4.h>
#include <core/quaternion.h>
#include <core/transform.h>
#include <core/vector3.h>
#include <graphics/cube_map.h>
#include <graphics/lights/point_light.h>
#include <graphics/mesh_manager.h>
#include <graphics/post_processing_description.h>
#include <graphics/render_graph/binary_operator_node.h>
#include <graphics/render_graph/blur_node.h>
#include <graphics/render_graph/camera_node.h>
#include <graphics/render_graph/colour_node.h>
#include <graphics/render_graph/combine_node.h>
#include <graphics/render_graph/component_node.h>
#include <graphics/render_graph/composite_node.h>
#include <graphics/render_graph/conditional_node.h>
#include <graphics/render_graph/fragment_node.h>
#include <graphics/render_graph/invert_node.h>
#include <graphics/render_graph/lerp_node.h>
#include <graphics/render_graph/property_node.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/render_node.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_graph/time_node.h>
#include <graphics/render_graph/unary_operator_node.h>
#include <graphics/render_graph/value_node.h>
#include <graphics/render_graph/variable_node.h>
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
iris::Vector3 calculate_velocity(iris::Camera &camera, const std::map<iris::Key, iris::KeyState> &key_map)
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

    velocity.y = 0.0f;
    return velocity;
}

void add_wave(float dx, float dy, float wavelength, float steepness, iris::RenderGraph *water_graph)
{
    const auto n_dx = dx / std::hypot(dx, dy);
    const auto n_dy = dy / std::hypot(dx, dy);

    static auto counter = 1u;

    water_graph->create_variable<iris::ValueNode<float>>(
        "wavelength" + std::to_string(counter), iris::VariableNodeType::FLOAT, true, wavelength);
    water_graph->create_variable<iris::ValueNode<float>>(
        "steepness" + std::to_string(counter), iris::VariableNodeType::FLOAT, true, steepness);

    water_graph->create_variable<iris::BinaryOperatorNode>(
        "k" + std::to_string(counter),
        iris::VariableNodeType::FLOAT,
        true,
        water_graph->create<iris::ValueNode<float>>(6.2831853f),
        water_graph->create<iris::VariableNode>("wavelength" + std::to_string(counter)),
        iris::BinaryOperator::DIVIDE);

    water_graph->create_variable<iris::BinaryOperatorNode>(
        "a" + std::to_string(counter),
        iris::VariableNodeType::FLOAT,
        true,
        water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
        water_graph->create<iris::VariableNode>("k" + std::to_string(counter)),
        iris::BinaryOperator::DIVIDE);

    water_graph->create_variable<iris::UnaryOperatorNode>(
        "c" + std::to_string(counter),
        iris::VariableNodeType::FLOAT,
        true,
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ValueNode<float>>(9.8f),
            water_graph->create<iris::VariableNode>("k" + std::to_string(counter)),
            iris::BinaryOperator::DIVIDE),
        iris::UnaryOperator::SQUARE_ROOT);

    water_graph->create_variable<iris::CombineNode>(
        "d" + std::to_string(counter),
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::ValueNode<float>>(n_dx),
        water_graph->create<iris::ValueNode<float>>(n_dy),
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f));

    water_graph->create_variable<iris::BinaryOperatorNode>(
        "f" + std::to_string(counter),
        iris::VariableNodeType::FLOAT,
        true,
        water_graph->create<iris::VariableNode>("k" + std::to_string(counter)),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::ComponentNode>(
                    water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".xy"),
                water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".xz"),
                iris::BinaryOperator::DOT),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::VariableNode>("c" + std::to_string(counter)),
                water_graph->create<iris::TimeNode>(),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::SUBTRACT),
        iris::BinaryOperator::MULTIPLY);

    water_graph->create_variable<iris::CombineNode>(
        "t" + std::to_string(counter),
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ValueNode<float>>(1.0f),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".x"),
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".x"),
                    iris::BinaryOperator::MULTIPLY),
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::UnaryOperatorNode>(
                        water_graph->create<iris::VariableNode>("f" + std::to_string(counter)),
                        iris::UnaryOperator::SIN),
                    water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
                    iris::BinaryOperator::MULTIPLY),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::SUBTRACT),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ComponentNode>(
                water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".x"),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::UnaryOperatorNode>(
                    water_graph->create<iris::VariableNode>("f" + std::to_string(counter)), iris::UnaryOperator::COS),
                water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::MULTIPLY),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::UnaryOperatorNode>(
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".x"),
                    iris::UnaryOperator::NEGATE),
                water_graph->create<iris::ComponentNode>(
                    water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".y"),
                iris::BinaryOperator::MULTIPLY),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::UnaryOperatorNode>(
                    water_graph->create<iris::VariableNode>("f" + std::to_string(counter)), iris::UnaryOperator::SIN),
                water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::MULTIPLY),
        water_graph->create<iris::ValueNode<float>>(0.0f));

    water_graph->create_variable<iris::CombineNode>(
        "b" + std::to_string(counter),
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::UnaryOperatorNode>(
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".x"),
                    iris::UnaryOperator::NEGATE),
                water_graph->create<iris::ComponentNode>(
                    water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".y"),
                iris::BinaryOperator::MULTIPLY),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::UnaryOperatorNode>(
                    water_graph->create<iris::VariableNode>("f" + std::to_string(counter)), iris::UnaryOperator::SIN),
                water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::MULTIPLY),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ComponentNode>(
                water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".y"),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::UnaryOperatorNode>(
                    water_graph->create<iris::VariableNode>("f" + std::to_string(counter)), iris::UnaryOperator::COS),
                water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::MULTIPLY),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ValueNode<float>>(1.0f),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".y"),
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".y"),
                    iris::BinaryOperator::MULTIPLY),
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::UnaryOperatorNode>(
                        water_graph->create<iris::VariableNode>("f" + std::to_string(counter)),
                        iris::UnaryOperator::SIN),
                    water_graph->create<iris::VariableNode>("steepness" + std::to_string(counter)),
                    iris::BinaryOperator::MULTIPLY),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::SUBTRACT),
        water_graph->create<iris::ValueNode<float>>(0.0f));

    water_graph->create_variable<iris::UnaryOperatorNode>(
        "tb" + std::to_string(counter),
        iris::VariableNodeType::VEC3,
        true,
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ComponentNode>(
                water_graph->create<iris::VariableNode>("b" + std::to_string(counter)), ".xyz"),
            water_graph->create<iris::ComponentNode>(
                water_graph->create<iris::VariableNode>("t" + std::to_string(counter)), ".xyz"),
            iris::BinaryOperator::CROSS),
        iris::UnaryOperator::NORMALISE);

    water_graph->create_variable<iris::CombineNode>(
        "n" + std::to_string(counter),
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::ComponentNode>(
            water_graph->create<iris::VariableNode>("tb" + std::to_string(counter)), ".x"),
        water_graph->create<iris::ComponentNode>(
            water_graph->create<iris::VariableNode>("tb" + std::to_string(counter)), ".y"),
        water_graph->create<iris::ComponentNode>(
            water_graph->create<iris::VariableNode>("tb" + std::to_string(counter)), ".z"),
        water_graph->create<iris::ValueNode<float>>(0.0f));

    water_graph->create_variable<iris::CombineNode>(
        "p" + std::to_string(counter),
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".x"),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::ComponentNode>(
                    water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".x"),
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::VariableNode>("a" + std::to_string(counter)),
                    water_graph->create<iris::UnaryOperatorNode>(
                        water_graph->create<iris::VariableNode>("f" + std::to_string(counter)),
                        iris::UnaryOperator::COS),
                    iris::BinaryOperator::MULTIPLY),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::ADD),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::VariableNode>("a" + std::to_string(counter)),
            water_graph->create<iris::UnaryOperatorNode>(
                water_graph->create<iris::VariableNode>("f" + std::to_string(counter)), iris::UnaryOperator::SIN),
            iris::BinaryOperator::MULTIPLY),
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".z"),
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::ComponentNode>(
                    water_graph->create<iris::VariableNode>("d" + std::to_string(counter)), ".y"),
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::VariableNode>("a" + std::to_string(counter)),
                    water_graph->create<iris::UnaryOperatorNode>(
                        water_graph->create<iris::VariableNode>("f" + std::to_string(counter)),
                        iris::UnaryOperator::COS),
                    iris::BinaryOperator::MULTIPLY),
                iris::BinaryOperator::MULTIPLY),
            iris::BinaryOperator::ADD),
        water_graph->create<iris::ValueNode<float>>(1.0f));

    water_graph->create_variable<iris::BinaryOperatorNode>(
        "p",
        iris::VariableNodeType::VEC4,
        false,
        water_graph->create<iris::VariableNode>("p"),
        water_graph->create<iris::VariableNode>("p" + std::to_string(counter)),
        iris::BinaryOperator::ADD);

    water_graph->create_variable<iris::BinaryOperatorNode>(
        "n",
        iris::VariableNodeType::VEC4,
        false,
        water_graph->create<iris::VariableNode>("n"),
        water_graph->create<iris::VariableNode>("n" + std::to_string(counter)),
        iris::BinaryOperator::ADD);

    ++counter;
}

iris::Vector3 wave_height(const iris::Vector3 &p, float dx, float dy, float wavelength, float steepness, float time)
{
    const auto d = iris::Vector3::normalise({dx, dy, 0.0f});
    const auto k = (std::numbers::pi_v<float> * 2.0f) / wavelength;
    const auto c = std::sqrt(9.8f / k);
    const auto f = k * (d.dot(iris::Vector3{p.x, p.z, 0.0f}) - c * time);
    const auto a = steepness / k;

    return {d.x * (a * std::cos(f)), a * std::sin(f), d.y * (a * std::cos(f))};
}

std::vector<std::tuple<float, float, float, float>> waves{
    {1.0f, 1.0f, 60.0f, 0.15f},
    {1.0f, 0.6f, 30.0f, 0.15f},
    {1.0f, 1.3f, 20.0f, 0.15f},
};

}

WaterSample::WaterSample(iris::Window *window, iris::RenderPipeline &render_pipeline, iris::Context &context)
    : light_transform_()
    , light_(nullptr)
    , camera_(iris::CameraType::PERSPECTIVE, window->width(), window->height(), 10000u)
    , key_map_()
    , sky_box_(nullptr)
    , scene_(nullptr)
    , player_()
    , azimuth_(std::numbers::pi_v<float> / 2.0f)
    , altitude_(std::numbers::pi_v<float> / 8.0f)
    , camera_distance_(200.0f)
    , render_target_(nullptr)
    , water_offset_x_property_()
    , water_offset_y_property_()
    , water_offset_x_(0.0f)
    , water_offset_y_(0.0f)
    , water_(nullptr)
    , window_(window)
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

    auto &mesh_manager = context.mesh_manager();
    auto &texture_manager = context.texture_manager();
    auto &render_target_manager = context.render_target_manager();

    sky_box_ =
        texture_manager.create(iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    auto *water_graph = render_pipeline.create_render_graph();

    water_offset_x_property_ = water_graph->create_property<float>("water_offset_x", water_offset_x_);
    water_offset_y_property_ = water_graph->create_property<float>("water_offset_y", water_offset_y_);

    // create variables to store vertex position and normal

    water_graph->create_variable<iris::CombineNode>(
        "p",
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f));

    water_graph->create_variable<iris::CombineNode>(
        "n",
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f),
        water_graph->create<iris::ValueNode<float>>(0.0f));

    // add all waves to graph
    for (const auto &[dx, dy, wavelength, steepness] : waves)
    {
        add_wave(dx, dy, wavelength, steepness, water_graph);
    }

    const auto flow_map = texture_manager.load("FlowMap2.png", iris::TextureUsage::DATA);

    auto *uv_node = water_graph->create<iris::BinaryOperatorNode>(
        water_graph->create<iris::BinaryOperatorNode>(
            water_graph->create<iris::ComponentNode>(
                water_graph->create<iris::CombineNode>(
                    water_graph->create<iris::PropertyNode>("water_offset_x"),
                    water_graph->create<iris::PropertyNode>("water_offset_y"),
                    water_graph->create<iris::ValueNode<float>>(0.0f),
                    water_graph->create<iris::ValueNode<float>>(0.0f)),
                ".xy"),
            water_graph->create<iris::FragmentNode>(iris::FragmentDataType::TEX_COORD),
            iris::BinaryOperator::ADD),
        water_graph->create<iris::TextureNode>(
            flow_map,
            iris::UVSource::NODE,
            water_graph->create<iris::BinaryOperatorNode>(
                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::ComponentNode>(
                        water_graph->create<iris::CombineNode>(
                            water_graph->create<iris::PropertyNode>("water_offset_x"),
                            water_graph->create<iris::PropertyNode>("water_offset_y"),
                            water_graph->create<iris::ValueNode<float>>(0.0f),
                            water_graph->create<iris::ValueNode<float>>(0.0f)),
                        ".xy"),
                    water_graph->create<iris::BinaryOperatorNode>(
                        water_graph->create<iris::TimeNode>(),
                        water_graph->create<iris::ValueNode<float>>(0.01f),
                        iris::BinaryOperator::MULTIPLY),
                    iris::BinaryOperator::ADD),
                water_graph->create<iris::FragmentNode>(iris::FragmentDataType::TEX_COORD),
                iris::BinaryOperator::ADD)),
        iris::BinaryOperator::ADD);

    static constexpr iris::Colour water_base_colour{0.17f, 0.53f, 0.54f};
    static constexpr iris::Colour water_dark_colour{0.12f, 0.22f, 0.22f};
    static constexpr iris::Colour water_light_colour{1.0f, 1.0f, 1.0f};

    const auto *voronoi = texture_manager.load("Voronoi2.png");

    water_graph->create_variable<iris::BinaryOperatorNode>(
        "dist",
        iris::VariableNodeType::FLOAT,
        true,
        water_graph->create<iris::FragmentNode>(iris::FragmentDataType::VIEW_POSITION, ".xyz"),
        water_graph->create<iris::CameraNode>(iris::CameraDataType::POSITION, ".xyz"),
        iris::BinaryOperator::DISTANCE);

    water_graph->create_variable<iris::LerpNode>(
        "water_colour",
        iris::VariableNodeType::VEC4,
        true,
        water_graph->create<iris::LerpNode>(
            water_graph->create<iris::ColourNode>(water_base_colour),
            water_graph->create<iris::ColourNode>(water_dark_colour),
            water_graph->create<iris::TextureNode>(voronoi, iris::UVSource::NODE, uv_node)),
        water_graph->create<iris::ColourNode>(water_light_colour),
        water_graph->create<iris::TextureNode>(
            voronoi,
            iris::UVSource::NODE,
            water_graph->create<iris::BinaryOperatorNode>(
                uv_node,
                water_graph->create<iris::ComponentNode>(
                    water_graph->create<iris::CombineNode>(
                        water_graph->create<iris::ValueNode<float>>(0.1f),
                        water_graph->create<iris::ValueNode<float>>(0.1f),
                        water_graph->create<iris::ValueNode<float>>(0.0f),
                        water_graph->create<iris::ValueNode<float>>(0.0f)),
                    ".xy"),
                iris::BinaryOperator::ADD)));

    static const auto min_fog_distance = 2000.0f;
    static const auto max_fog_distance = 3000.0f;

    water_graph->render_node()->set_position_input(water_graph->create<iris::VariableNode>("p"));
    water_graph->render_node()->set_vertex_normal_input(water_graph->create<iris::VariableNode>("n"));
    water_graph->render_node()->set_colour_input(water_graph->create<iris::ConditionalNode>(
        water_graph->create<iris::VariableNode>("dist"),
        water_graph->create<iris::ValueNode<float>>(min_fog_distance),
        water_graph->create<iris::VariableNode>("water_colour"),
        water_graph->create<iris::ConditionalNode>(
            water_graph->create<iris::VariableNode>("dist"),
            water_graph->create<iris::ValueNode<float>>(max_fog_distance),
            water_graph->create<iris::ColourNode>(iris::Colour{water_base_colour, 0.0f}),

            water_graph->create<iris::LerpNode>(
                water_graph->create<iris::VariableNode>("water_colour"),
                water_graph->create<iris::ColourNode>(iris::Colour{water_base_colour, 0.0f}),

                water_graph->create<iris::BinaryOperatorNode>(
                    water_graph->create<iris::BinaryOperatorNode>(
                        water_graph->create<iris::VariableNode>("dist"),
                        water_graph->create<iris::ValueNode<float>>(min_fog_distance),
                        iris::BinaryOperator::SUBTRACT),
                    water_graph->create<iris::ValueNode<float>>(max_fog_distance - min_fog_distance),
                    iris::BinaryOperator::DIVIDE)),

            iris::ConditionalOperator::GREATER),
        iris::ConditionalOperator::LESS));

    auto *water_graph2 = render_pipeline.create_render_graph();

    water_graph2->create_variable<iris::ValueNode<float>>("a", iris::VariableNodeType::FLOAT, true, 0.5f);
    water_graph2->create_variable<iris::ValueNode<float>>("s", iris::VariableNodeType::FLOAT, true, 5.0f);
    water_graph2->create_variable<iris::ValueNode<float>>("w", iris::VariableNodeType::FLOAT, true, 10.0f);
    water_graph2->create_variable<iris::BinaryOperatorNode>(
        "k",
        iris::VariableNodeType::FLOAT,
        true,
        water_graph2->create<iris::ValueNode<float>>(std::numbers::pi_v<float> * 2.0f),
        water_graph2->create<iris::VariableNode>("w"),
        iris::BinaryOperator::DIVIDE);

    water_graph2->create_variable<iris::BinaryOperatorNode>(
        "new_y",
        iris::VariableNodeType::FLOAT,
        true,
        water_graph2->create<iris::VariableNode>("a"),
        water_graph2->create<iris::UnaryOperatorNode>(
            water_graph2->create<iris::BinaryOperatorNode>(
                water_graph2->create<iris::VariableNode>("k"),
                water_graph2->create<iris::BinaryOperatorNode>(
                    water_graph2->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".x"),
                    water_graph2->create<iris::BinaryOperatorNode>(
                        water_graph2->create<iris::VariableNode>("s"),
                        water_graph2->create<iris::TimeNode>(),
                        iris::BinaryOperator::MULTIPLY),
                    iris::BinaryOperator::SUBTRACT),
                iris::BinaryOperator::MULTIPLY),
            iris::UnaryOperator::SIN),
        iris::BinaryOperator::MULTIPLY);

    water_graph2->render_node()->set_position_input(water_graph2->create<iris::CombineNode>(
        water_graph2->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".x"),
        water_graph2->create<iris::VariableNode>("new_y"),
        water_graph2->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".z"),
        water_graph2->create<iris::VertexNode>(iris::VertexDataType::POSITION, ".w")));

    water_ = scene_->create_entity<iris::SingleEntity>(
        water_graph,
        mesh_manager.plane({0.0f, 0.5f, 1.0f}, 100, 200.0f),
        iris::Transform{{0.0f, 0.0f, 0.0f}, {}, {20.0f}},
        true);

    const iris::Vector3 light_pos{0.0f, 5.0f, 0.0f};

    light_ = scene_->create_light<iris::DirectionalLight>(
        iris::Vector3{1.0f, -1.0f, 0.0f}, iris::Colour{0.3f, 0.3f, 0.3f, 1.0f});

    auto ship_meshes = mesh_manager.load_mesh("ship.fbx");
    auto *ship_graph = render_pipeline.create_render_graph();
    const auto *ship_texture = texture_manager.load("ship_texture.png");
    ship_graph->render_node()->set_colour_input(ship_graph->create<iris::TextureNode>(ship_texture));

    for (const auto &mesh : ship_meshes.mesh_data)
    {
        auto *entity = scene_->create_entity<iris::SingleEntity>(
            ship_graph, mesh.mesh, iris::Transform{{0.0f, 40.0f, 0.0f}, {}, {20.0f}});
        entity->set_wireframe(false);

        player_.push_back({entity, entity->position()});
    }

    auto *island_graph = render_pipeline.create_render_graph();

    island_graph->create_variable<iris::BinaryOperatorNode>(
        "dist",
        iris::VariableNodeType::FLOAT,
        true,
        island_graph->create<iris::FragmentNode>(iris::FragmentDataType::VIEW_POSITION, ".xyz"),
        island_graph->create<iris::CameraNode>(iris::CameraDataType::POSITION, ".xyz"),
        iris::BinaryOperator::DISTANCE);

    island_graph->create_variable<iris::ColourNode>(
        "island_colour", iris::VariableNodeType::VEC4, true, iris::Colour{1.0f, 1.0f, 1.0f, 1.0f});

    static const auto min_fade_distance = 3000.0f;
    static const auto max_fade_distance = 4000.0f;

    island_graph->render_node()->set_colour_input(island_graph->create<iris::ConditionalNode>(
        island_graph->create<iris::VariableNode>("dist"),
        island_graph->create<iris::ValueNode<float>>(min_fade_distance),
        island_graph->create<iris::VariableNode>("island_colour"),
        island_graph->create<iris::ConditionalNode>(
            island_graph->create<iris::VariableNode>("dist"),
            island_graph->create<iris::ValueNode<float>>(max_fade_distance),
            island_graph->create<iris::ColourNode>(iris::Colour{0.0f, 0.0f, 0.0f, 0.0f}),

            island_graph->create<iris::LerpNode>(
                island_graph->create<iris::VariableNode>("island_colour"),
                island_graph->create<iris::ColourNode>(iris::Colour{0.0f, 0.0f, 0.0f, 0.0f}),

                island_graph->create<iris::BinaryOperatorNode>(
                    island_graph->create<iris::BinaryOperatorNode>(
                        island_graph->create<iris::VariableNode>("dist"),
                        island_graph->create<iris::ValueNode<float>>(min_fade_distance),
                        iris::BinaryOperator::SUBTRACT),
                    island_graph->create<iris::ValueNode<float>>(max_fade_distance - min_fade_distance),
                    iris::BinaryOperator::DIVIDE)),

            iris::ConditionalOperator::GREATER),
        iris::ConditionalOperator::LESS));

    island_ = scene_->create_entity<iris::SingleEntity>(
        island_graph,
        mesh_manager.load_mesh("sphere.fbx").mesh_data.front().mesh,
        iris::Transform{{0.0f, 900000.0f, -400.0f}, {}, {40.0f}},
        true);

    const auto mesh_parts = mesh_manager.load_mesh("scene.fbx");
    for (const auto &mesh : mesh_parts.mesh_data)
    {
        scene_->create_entity<iris::SingleEntity>(
            nullptr,
            mesh.mesh,
            iris::Transform{{0.0f, 30.0f, -400.0f}, {{1.0f, 0.0f, 0.0f}, -std::numbers::pi_v<float> / 2.0f}, {40.0f}},
            true);
    }

    auto *pass = render_pipeline.create_render_pass(scene_);
    pass->post_processing_description = {.ambient_occlusion = {iris::AmbientOcclusionDescription{}}};
    pass->colour_target = render_target_manager.create();
    pass->camera = &camera_;
    pass->sky_box = sky_box_;

    render_target_ = pass->colour_target;
}

void WaterSample::fixed_update()
{
}

void WaterSample::variable_update()
{
    const auto velocity = calculate_velocity(camera_, key_map_);
    player_position_ = player_position_ + velocity;

    water_->set_position({player_position_.x, 0.0f, player_position_.z});

    iris::Quaternion player_orientation{{0.0f, 0.0f, 1.0f}, -azimuth_};
    player_orientation.normalise();
    const auto mesh_rotate = iris::Quaternion{{1.0f, 0.0f, 0.0f}, -std::numbers::pi_v<float> / 2.0f} *
                             player_orientation *
                             iris::Quaternion{{0.0f, 0.0f, 1.0f}, -std::numbers::pi_v<float> / 2.0f};

    for (auto &[entity, offset] : player_)
    {
        entity->set_position(player_position_ + offset);
        entity->set_orientation(mesh_rotate);
    }

    const iris::Vector3 offset{
        camera_distance_ * std::sin((std::numbers::pi_v<float> / 2.0f) - altitude_) * std::cos(azimuth_),
        camera_distance_ * std::cos((std::numbers::pi_v<float> / 2.0f) - altitude_),
        camera_distance_ * std::sin((std::numbers::pi_v<float> / 2.0f) - altitude_) * std::sin(azimuth_)};

    // update camera position to always follow player
    camera_.set_position(iris::Vector3{player_position_.x, 0.0f, player_position_.z} + offset);

    if (key_map_.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        const auto direction = camera_.direction();

        water_offset_x_ += direction.x * 0.1f;
        water_offset_y_ -= direction.z * 0.1f;

        water_offset_x_property_.set_value(water_offset_x_);
        water_offset_y_property_.set_value(water_offset_y_);
    }

    // drop island at a distance
    const auto drop_amount = 70.0f;
    const auto min_drop_distance = 1800.0f;
    const auto max_drop_distance = 2000.0f;
    const auto distance = iris::Vector3::distance(player_position_, island_->position());

    auto drop = 0.0f;

    if (distance <= min_drop_distance)
    {
        drop = 0.0f;
    }
    else if (distance >= max_drop_distance)
    {
        drop = drop_amount;
    }
    else
    {
        drop = ((distance - min_drop_distance) / (max_drop_distance - min_drop_distance)) * drop_amount;
    }

    auto island_position = island_->position();
    island_position.y = -drop;
    island_->set_position(island_position);

    // calculate wave height on cpu and set player to it

    const iris::Vector3 wave_base{};
    iris::Vector3 new_pos{};

    for (const auto &[dx, dy, wavelength, steepness] : waves)
    {
        new_pos += wave_height(wave_base, dx, dy, wavelength, steepness, window_->time().count() / 1000.0f) * 20.0f;
    }

    player_position_.y = new_pos.y - 8.0f;
}

void WaterSample::handle_input(const iris::Event &event)
{
    if (event.is_key())
    {
        const auto keyboard = event.key();
        key_map_[keyboard.key] = keyboard.state;

        if (keyboard.key == iris::Key::SPACE)
        {
            for (auto [e, _] : player_)
            {
                auto p = e->position();
                p.y = 40.0f;
                e->set_position(p);
            }
        }
    }
    else if (event.is_mouse())
    {
        static const auto sensitivity = 0.0025f;
        const auto mouse = event.mouse();

        //  adjust camera azimuth and ensure we are still pointing at the player
        azimuth_ += mouse.delta_x * sensitivity;
        camera_.adjust_yaw(mouse.delta_x * sensitivity);

        static constexpr auto offset = 0.01f;

        // adjust camera altitude and ensure we are still pointing at the player
        // we clamp the altitude [0, pi/2] to ensure no weirdness happens
        altitude_ += mouse.delta_y * sensitivity;
        altitude_ = std::clamp(altitude_, 0.0f, (std::numbers::pi_v<float> / 2.0f) - offset);

        camera_.set_pitch(-altitude_);
    }
    else if (event.is_scroll_wheel())
    {
        const auto scroll = event.scroll_wheel();
        camera_distance_ += scroll.delta_y * -15.0f;
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
