////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <map>
#include <string>

#include <core/camera.h>
#include <core/transform.h>
#include <events/event.h>
#include <graphics/animation/animation_controller.h>
#include <graphics/cube_map.h>
#include <graphics/lights/directional_light.h>
#include <graphics/render_entity.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_pipeline.h>
#include <graphics/single_entity.h>
#include <graphics/skeleton.h>
#include <graphics/window.h>
#include <physics/physics_system.h>
#include <physics/rigid_body.h>

#include "sample.h"

/**
 * Sample showcasing animation.
 */
class AnimationSample : public Sample
{
  public:
    /**
     * Create a new AnimationSample.
     *
     * @param window
     *   Window to render with.
     *
     * @param target
     *   Target to render to.
     */
    AnimationSample(iris::Window *window, iris::RenderPipeline &render_pipeline);
    ~AnimationSample() override = default;

    /**
     * Fixed rate update function.
     */
    void fixed_update() override;

    /**
     * Variable rate update function.
     */
    void variable_update() override;

    /**
     * Handle a user input.
     *
     * @param event
     *   User input event.
     */
    void handle_input(const iris::Event &event) override;

    /**
     * Title of sample.
     *
     * @returns
     *   Sample title.
     */
    std::string title() const override;

    /**
     * Get the target the sample will render to.
     *
     * @returns
     *   Sample render target.
     */
    const iris::RenderTarget *target() const override;

  private:
    /** Transform for moving light. */
    iris::Transform light_transform_;

    /** Scene light */
    iris::DirectionalLight *light_;

    /** Render camera. */
    iris::Camera camera_;

    /** Physics system */
    iris::PhysicsSystem *physics_;

    /** Zombie entity. */
    iris::SingleEntity *zombie_;

    /** Current animation number. */
    std::size_t animation_;

    /** Collection of animation names. */
    std::vector<std::string> animations_;

    /** Mapping of bone name to index and rigid body. */
    std::map<std::string, std::tuple<std::size_t, iris::RigidBody *>> hit_boxes_;

    /** Mapping of bone name to offsets. */
    std::map<std::string, std::tuple<iris::Vector3, iris::Vector3>> hit_box_data_;

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;

    /** Sky box for sample. */
    iris::CubeMap *sky_box_;

    /** Mesh for debug rendering. */
    std::unique_ptr<iris::Mesh> debug_mesh_;

    /** Animation controller for zombie. */
    std::unique_ptr<iris::AnimationController> animation_controller_;

    /** Scene for sample. */
    iris::Scene *scene_;

    /** Render target for sample. */
    const iris::RenderTarget *render_target_;
};
