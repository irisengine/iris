////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <cstddef>
#include <map>
#include <string>

#include <core/camera.h>
#include <core/context.h>
#include <core/transform.h>
#include <events/event.h>
#include <graphics/animation/animation_controller.h>
#include <graphics/cube_map.h>
#include <graphics/lights/directional_light.h>
#include <graphics/render_entity.h>
#include <graphics/render_graph/property_writer.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_pipeline.h>
#include <graphics/single_entity.h>
#include <graphics/window.h>

#include "sample.h"

/**
 */
class WaterSample : public Sample
{
  public:
    /**
     * Create a new WaterSample.
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
    WaterSample(iris::Window *window, iris::RenderPipeline &render_pipeline, iris::Context &context);
    ~WaterSample() override = default;

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

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;

    /** Sky box for sample. */
    iris::CubeMap *sky_box_;

    /** Scene for sample. */
    iris::Scene *scene_;

    std::vector<std::tuple<iris::SingleEntity *, iris::Vector3>> player_;

    /** Camera azimuth. */
    float azimuth_;

    /** Camera altitude. */
    float altitude_;

    /** Distance to player. */
    float camera_distance_;

    /** Render target for sample. */
    const iris::RenderTarget *render_target_;

    /** Writer for water texture uv. */
    iris::PropertyWriter<float> water_offset_x_property_;

    /** Writer for water texture uv. */
    iris::PropertyWriter<float> water_offset_y_property_;

    /** Offset for water texture uv. */
    float water_offset_x_;

    /** Offset for water texture uv. */
    float water_offset_y_;

    /** Water entity. */
    iris::SingleEntity *water_;

    /** Player position. */
    iris::Vector3 player_position_;

    /** Island entity. */
    iris::SingleEntity *island_;

    /** Window object. */
    iris::Window *window_;
};
