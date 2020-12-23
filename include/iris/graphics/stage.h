#pragma once

#include <memory>
#include <vector>

#include <memory>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "graphics/light.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{

/**
 * Internal struct encapsulating all data needed by a RenderSystem to render.
 */
struct RenderItem
{
    /** Entity to render. */
    RenderEntity *entity;

    /** Material to render with. */
    Material *material;

    /** Lights effecting entity. */
    std::vector<Light *> lights;
};

/**
 * A stage is a single render pass of a scene, with a camera to a target.
 *
 * This is an internal class and shouldn't be created directly. Pipeline should
 * be used for creating render passes.
 *
 * Note this class doesn't do any rendering, it just encapsulates the
 * information required.
 */
class Stage
{
  public:
    /**
     * Create a stage to render a scene to the screen target.
     *
     * @param scene
     *   Scene to be rendered.
     *
     * @param camera
     *   Camera to render scene with.
     */
    Stage(Scene *scene, Camera &camera);

    /**
     * Create a stage to render a scene to a custom target.
     *
     * @param scene
     *   Scene to be rendered.
     *
     * @param camera
     *   Camera to render scene with.
     *
     * @param target
     *   Target to be rendered to.
     */
    Stage(Scene *scene, Camera &camera, RenderTarget &target);

    /**
     * Get a collection of RenderEntity's and associated Materials.
     *
     * @returns
     *   Collection of <RenderEntity, Material> tuples.
     */
    std::vector<RenderItem> render_items() const;

    /**
     * Get reference to camera.
     *
     * @returns
     *   Camera reference.
     */
    Camera &camera();

    /**
     * Get reference to render target.
     *
     * @returns
     *   RenderTarget reference.
     */
    RenderTarget &target();

  private:
    /** Scene to be rendered. */
    Scene *scene_;

    /** Camera to render scene with. */
    Camera &camera_;

    /** Target to render scene to. */
    RenderTarget &target_;

    /** Collection of RenderEntity's and associated Materials. */
    std::vector<RenderItem> render_items_;

    /** Collection of created Materials. */
    std::vector<std::unique_ptr<Material>> materials_;
};
}
