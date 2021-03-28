#pragma once

#include <map>
#include <string>

#include <core/camera.h>
#include <core/transform.h>
#include <core/window.h>
#include <events/event.h>
#include <graphics/lights/point_light.h>
#include <graphics/pipeline.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_target.h>

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
     * @param target
     *   Target to render to.
     */
    RenderGraphSample(iris::Window *window, iris::RenderTarget *target);
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

  private:
    /** Pointer to window. */
    iris::Window *window_;

    /** Render pipeline for sample. */
    iris::Pipeline pipeline_;

    /** Transform for moving light. */
    iris::Transform light_transform_;

    /** Scene light */
    iris::PointLight *light1_;

    /** Scene light */
    iris::PointLight *light2_;

    /** Render target. */
    iris::RenderTarget sphere1_rt_;

    /** Render target. */
    iris::RenderTarget sphere2_rt_;

    /** Render camera. */
    iris::Camera camera_;

    /** Composite camera. */
    iris::Camera screen_camera_;

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;
};
