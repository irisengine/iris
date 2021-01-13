#pragma once

#include <any>
#include <memory>
#include <vector>

#include "core/camera.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"
#include "graphics/stage.h"

namespace iris
{
/**
 * A pipeline is a series of stages which, when rendered, constitute a final
 * frame to be displayed. In other words once a pipeline has been executed the
 * default screen target can be presented to the window and will show the
 * final rendered frame.
 *
 * The pipeline itself doesn't do any rendering, it is passed to the render
 * system, which then executes it and displays the default screen target.
 *
 * A pipeline may:
 *  - add stages
 *  - reorder stages
 *  - execute stages in parallel
 * as long as the final output remains the same.
 */
class Pipeline
{
  public:
    /**
     * Add a stage, which renders to the default screen target.
     *
     * @param scene
     *   Scene to render
     *
     * @param camera
     *   Camera to render scene with.
     */
    void add_stage(std::unique_ptr<Scene> scene, Camera &camera);

    /**
     * Add a staged, which renders to a custom target.
     *
     * @param scene
     *   Scene to render
     *
     * @param camera
     *   Camera to render scene with.
     *
     * @param target
     *   Target to render scene to.
     */
    void add_stage(
        std::unique_ptr<Scene> scene,
        Camera &camera,
        RenderTarget *target);

    /**
     * Get a reference to the stages.
     *
     * @returns
     *   Reference to collection of stages.
     */
    const std::vector<std::unique_ptr<Stage>> &stages() const;

  private:
    /** Collection of scenes. */
    std::vector<std::unique_ptr<Scene>> scenes_;

    /** Collection of stages. */
    std::vector<std::unique_ptr<Stage>> stages_;
};
}
