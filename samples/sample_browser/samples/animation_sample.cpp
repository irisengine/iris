////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "animation_sample.h"

#include <cmath>
#include <map>
#include <memory>
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
#include <graphics/lights/point_light.h>
#include <graphics/mesh_manager.h>
#include <graphics/post_processing_description.h>
#include <graphics/render_graph/binary_operator_node.h>
#include <graphics/render_graph/blur_node.h>
#include <graphics/render_graph/colour_node.h>
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

AnimationSample::AnimationSample(iris::Window *window, iris::RenderPipeline &render_pipeline)
    : light_transform_()
    , light_(nullptr)
    , camera_(iris::CameraType::PERSPECTIVE, window->width(), window->height())
    , physics_(iris::Root::physics_manager().create_physics_system())
    , zombie_(nullptr)
    , animation_(0u)
    , animations_()
    , hit_boxes_()
    , hit_box_data_()
    , key_map_()
    , sky_box_(nullptr)
    , debug_mesh_(nullptr)
    , animation_controller_()
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

    auto *floor_graph = render_pipeline.create_render_graph();

    floor_graph->render_node()->set_specular_amount_input(floor_graph->create<iris::ValueNode<float>>(0.0f));
    floor_graph->render_node()->set_colour_input(floor_graph->create<iris::ColourNode>(iris::Colour{0.9f, 0.9f, 0.9f}));

    auto &mesh_manager = iris::Root::mesh_manager();

    scene_->create_entity<iris::SingleEntity>(
        floor_graph,
        mesh_manager.cube({1.0f, 1.0f, 1.0f}),
        iris::Transform{iris::Vector3{0.0f, -500.0f, 0.0f}, {}, iris::Vector3{500.0f}});

    auto meshes = mesh_manager.load_mesh("Zombie.fbx");
    iris::ensure(meshes.mesh_data.size() == 1u, "expecting single mesh");

    auto [mesh, texture_name] = meshes.mesh_data.front();

    auto *render_graph = render_pipeline.create_render_graph();
    render_graph->render_node()->set_colour_input(render_graph->create<iris::TextureNode>(texture_name));
    zombie_ = scene_->create_entity<iris::SingleEntity>(
        render_graph,
        mesh,
        iris::Transform{iris::Vector3{0.0f, 0.0f, 0.0f}, {}, iris::Vector3{0.05f}},
        meshes.skeleton);

    zombie_->set_receive_shadow(false);

    sky_box_ = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    light_ = scene_->create_light<iris::DirectionalLight>(iris::Vector3{-1.0f, -1.0f, 0.0f}, true);
    light_transform_ = iris::Transform{light_->direction(), {}, {1.0f}};

    auto *pass = render_pipeline.create_render_pass(scene_);
    pass->post_processing_description = {.ambient_occlusion = {iris::AmbientOcclusionDescription{}}};
    pass->colour_target = iris::Root::render_target_manager().create();
    pass->camera = &camera_;
    pass->sky_box = sky_box_;

    render_target_ = pass->colour_target;

    animations_ = {
        "Zombie|ZombieWalk", "Zombie|ZombieBite", "Zombie|ZombieCrawl", "Zombie|ZombieIdle", "Zombie|ZombieRun"};

    animation_controller_ = std::make_unique<iris::AnimationController>(
        meshes.animations,
        std::vector<iris::AnimationLayer>{
            {{{animations_[0], animations_[1], 500ms},
              {animations_[1], animations_[2], 500ms},
              {animations_[2], animations_[3], 500ms},
              {animations_[3], animations_[4], 500ms},
              {animations_[4], animations_[0], 500ms}},
             animations_.front()}},
        zombie_->skeleton());

    // offsets and scales for bones we want to add rigid bodies to, these were
    // all handcrafted
    hit_box_data_ = {
        {"Head", {{}, {1.0f}}},
        {"HeadTop_End", {{0.0f, -0.2f, 0.0f}, {1.0f}}},

        {"RightArm", {{}, {1.0f}}},
        {"RightForeArm", {{}, {1.0f, 2.5f, 1.0f}}},
        {"RightHand", {{}, {1.0f}}},

        {"LeftArm", {{}, {1.0f}}},
        {"LeftForeArm", {{}, {1.0f, 2.5f, 1.0f}}},
        {"LeftHand", {{}, {1.0f}}},

        {"Spine", {{}, {2.0f, 1.0f, 1.0f}}},
        {"Spine1", {{}, {2.0f, 1.0f, 1.0f}}},
        {"Spine2", {{}, {2.0f, 1.0f, 1.0f}}},
        {"Hips", {{}, {1.0f}}},

        {"LeftUpLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 2.5f, 1.0f}}},
        {"LeftLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 3.0f, 1.0f}}},
        {"LeftFoot", {{}, {1.0f, 2.5f, 1.0f}}},

        {"RightUpLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 2.5f, 1.0f}}},
        {"RightLeg", {{0.0f, 0.6f, 0.0f}, {1.0f, 3.0f, 1.0f}}},
        {"RightFoot", {{}, {1.0f, 2.5f, 1.0f}}}};

    // iterate all bones and create hit boxes for those we have data for
    for (auto i = 0u; i < zombie_->skeleton()->bones().size(); ++i)
    {
        const auto &bone = zombie_->skeleton()->bone(i);

        const auto box_data = hit_box_data_.find(bone.name());
        if (box_data != std::end(hit_box_data_))
        {
            const auto &[pos_offset, scale_offset] = box_data->second;

            // get bone transform in world space
            const auto transform = iris::Transform{
                zombie_->transform() * zombie_->skeleton()->transforms()[i] * iris::Matrix4::invert(bone.offset())};

            // create hit box
            hit_boxes_[box_data->first] = {
                i,
                physics_->create_rigid_body(
                    iris::Vector3{}, physics_->create_box_collision_shape(scale_offset), iris::RigidBodyType::GHOST)};

            // calculate hit box location after offset is applied
            const auto offset = transform * iris::Matrix4::make_translate(pos_offset);

            // update hit box
            std::get<1>(hit_boxes_[box_data->first])->reposition(offset.translation(), transform.rotation());
            std::get<1>(hit_boxes_[box_data->first])->set_name(box_data->first);
        }
    }

    fixed_update();
}

void AnimationSample::fixed_update()
{
    update_camera(camera_, key_map_);

    light_transform_.set_matrix(
        iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) * light_transform_.matrix());
    light_->set_direction(light_transform_.translation());

    physics_->step(13ms);
}

void AnimationSample::variable_update()
{
    animation_controller_->update();

    // update hit boxes
    for (auto &[name, data] : hit_boxes_)
    {
        auto &[index, body] = data;

        // get hotbox transform in world space
        const auto transform = iris::Transform{
            zombie_->transform() * zombie_->skeleton()->transforms()[index] *
            iris::Matrix4::invert(zombie_->skeleton()->bone(index).offset())};
        const auto offset = transform * iris::Matrix4::make_translate(std::get<0>(hit_box_data_[name]));

        body->reposition(offset.translation(), transform.rotation());
    }
}

void AnimationSample::handle_input(const iris::Event &event)
{
    if (event.is_key())
    {
        const auto keyboard = event.key();
        key_map_[keyboard.key] = keyboard.state;

        if ((keyboard.key == iris::Key::SPACE) && (keyboard.state == iris::KeyState::UP))
        {
            animation_ = (animation_ + 1u) % animations_.size();
            animation_controller_->play(0u, animations_[animation_]);
        }
    }
    else if (event.is_mouse())
    {
        static const auto sensitivity = 0.0025f;
        const auto mouse = event.mouse();

        camera_.adjust_yaw(mouse.delta_x * sensitivity);
        camera_.adjust_pitch(-mouse.delta_y * sensitivity);
    }
}

std::string AnimationSample::title() const
{
    return "Animation";
}

const iris::RenderTarget *AnimationSample::target() const
{
    return render_target_;
}
