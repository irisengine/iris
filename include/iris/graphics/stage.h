#pragma once

#include <memory>
#include <vector>

#include <memory>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{
/**
 * A stage is a single render pass of a scene, with a camera to a target.
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
    Stage(std::unique_ptr<Scene> scene, Camera &camera);

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
    Stage(std::unique_ptr<Scene> scene, Camera &camera, RenderTarget &target);

    /**
     * Get a collection of RenderEntity's and associated Materials.
     *
     * @returns
     *   Collection of <RenderEntity, Material> tuples.
     */
    std::vector<std::tuple<RenderEntity *, Material *>> render_items() const;

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
    std::unique_ptr<Scene> scene_;

    /** Camera to render scene with. */
    Camera &camera_;

    /** Target to render scene to. */
    RenderTarget &target_;

    /** Collection of RenderEntity's and associated Materials. */
    std::vector<std::tuple<RenderEntity *, Material *>> render_items_;

    /** Collection of created Materials. */
    std::vector<std::unique_ptr<Material>> materials_;
};
}
