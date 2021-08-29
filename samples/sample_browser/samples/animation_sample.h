#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include <core/camera.h>
#include <core/transform.h>
#include <events/event.h>
#include <graphics/lights/directional_light.h>
#include <graphics/render_entity.h>
#include <graphics/render_graph/render_graph.h>
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
    AnimationSample(iris::Window *window, iris::RenderTarget *target);
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

    iris::RenderTarget *target_;

    iris::Scene scene_;

    /** Transform for moving light. */
    iris::Transform light_transform_;

    /** Scene light */
    iris::DirectionalLight *light_;

    /** Render camera. */
    iris::Camera camera_;

    /** Physics system */
    iris::PhysicsSystem physics_;

    /** Zombie entity. */
    iris::RenderEntity *zombie_;

    /** Current animation number. */
    std::size_t animation_;

    /** Collection of animation names. */
    std::vector<std::string> animations_;

    /** Mapping of bone name to index and rigid body. */
    std::map<std::string, std::tuple<std::size_t, iris::RigidBody *>>
        hit_boxes_;

    /** Mapping of bone name to offsets. */
    std::map<std::string, std::tuple<iris::Vector3, iris::Vector3>>
        hit_box_data_;

    /** User input key map. */
    std::map<iris::Key, iris::KeyState> key_map_;
};
