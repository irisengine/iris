#pragma once

#include <cstddef>
#include <map>
#include <string>

#include <core/camera.h>
#include <core/transform.h>
#include <events/event.h>
#include <graphics/lights/point_light.h>
#include <graphics/render_entity.h>
#include <graphics/render_graph/render_graph.h>
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
    PhysicsSample(iris::Window *window, iris::RenderTarget *target);
    ~PhysicsSample() override = default;

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
    std::vector<std::tuple<iris::RenderEntity *, iris::RigidBody *>> boxes_;

    /** Character controller. */
    iris::CharacterController *character_controller_;
};
