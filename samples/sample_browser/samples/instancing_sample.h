////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include <core/camera.h>
#include <core/transform.h>
#include <events/event.h>
#include <graphics/animation/animation_controller.h>
#include <graphics/cube_map.h>
#include <graphics/lights/directional_light.h>
#include <graphics/render_entity.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/window.h>
#include <physics/physics_system.h>
#include <physics/rigid_body.h>

#include "sample.h"

/**
 * Sample showcasing instancing.
 */
class InstancingSample : public Sample
{
  public:
    /**
     * Create a new InstancingSample.
     *
     * @param window
     *   Window to render with.
     *
     * @param target
     *   Target to render to.
     */
    InstancingSample(iris::Window *window, iris::RenderTarget *target);
    ~InstancingSample() override = default;

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

    std::vector<iris::RenderPass> render_passes() override;

    /**
     * Title of sample.
     *
     * @returns
     *   Sample title.
     */
    std::string title() const override;

  private:
    /** Pointer to window. */
    iris::Window *window_;

    iris::PhysicsSystem *ps_;

    iris::RenderTarget *target_;

    iris::Scene scene_;

    iris::PointLight *light_;

    /** Render camera. */
    iris::Camera camera_;

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;

    iris::CubeMap *sky_box_;

    std::unique_ptr<iris::Mesh> debug_mesh_;
};
