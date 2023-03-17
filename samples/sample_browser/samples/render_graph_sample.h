////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <deque>
#include <map>
#include <string>

#include <core/camera.h>
#include <core/context.h>
#include <core/transform.h>
#include <events/event.h>
#include <graphics/cube_map.h>
#include <graphics/lights/point_light.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_pipeline.h>
#include <graphics/render_target.h>
#include <graphics/window.h>

#include "sample.h"

/**
 * Sample showcasing the render graph.
 */
class RenderGraphSample : public Sample
{
  public:
    /**
     * Create a new RenderGraphSample.
     *
     * @param window
     *   Window to render with.
     *
     * @param render_pipeline
     *   Render pipeline to use for sample.
     *
     * @param context
     *   Engine context object.
     */
    RenderGraphSample(iris::Window *window, iris::RenderPipeline &render_pipeline, iris::Context &context);
    ~RenderGraphSample() override = default;

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
    iris::PointLight *light1_;

    /** Scene light */
    iris::PointLight *light2_;

    /** Render camera. */
    iris::Camera camera_;

    /** Composite camera. */
    iris::Camera screen_camera_;

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;

    /** Sky box for scene. */
    iris::CubeMap *sky_box_;

    /** Render target for scene. */
    const iris::RenderTarget *render_target_;
};
