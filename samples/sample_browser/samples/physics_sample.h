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
#include <graphics/cube_map.h>
#include <graphics/lights/point_light.h>
#include <graphics/render_entity.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_pipeline.h>
#include <graphics/single_entity.h>
#include <graphics/window.h>
#include <physics/character_controller.h>
#include <physics/physics_system.h>
#include <physics/rigid_body.h>

#include "sample.h"

/**
 * Sample showcasing the physics engine.
 */
class PhysicsSample : public Sample
{
  public:
    /**
     * Create a new PhysicsSample.
     *
     * @param window
     *   Window to render with.
     *
     * @param target
     *   Target to render to.
     */
    PhysicsSample(iris::Window *window, iris::RenderPipeline &render_pipeline);

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
    /** Render target for sample. */
    const iris::RenderTarget *render_target_;

    /** Physics system */
    iris::PhysicsSystem *physics_;

    /** Transform for moving light. */
    iris::Transform light_transform_;

    /** Scene light */
    iris::PointLight *light_;

    /** Render camera. */
    iris::Camera camera_;

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;

    /** Collection of render entity and rigid body pairs. */
    std::vector<std::tuple<iris::SingleEntity *, iris::RigidBody *>> boxes_;

    /** Character controller. */
    iris::CharacterController *character_controller_;
};
