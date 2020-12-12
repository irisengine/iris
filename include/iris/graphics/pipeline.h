#pragma once

#include <any>
#include <memory>
#include <vector>

#include "graphics/stage.h"

namespace iris
{
/**
 * A pipeline is a series of Stages which, when rendered, constitute a final
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
     * Create a pipeline with a single stage.
     *
     * @param stage
     *   Single stage.
     */
    Pipeline(std::unique_ptr<Stage> stage);

    /**
     * Create a pipeline with multiple stages.
     *
     * @param stages
     *   Stages of pipeline.
     */
    Pipeline(std::vector<std::unique_ptr<Stage>> stages);

    /**
     * Get a reference to the stages.
     *
     * @returns
     *   Reference to collection of stages.
     */
    std::vector<std::unique_ptr<Stage>> &stages();

  private:
    /** Collection of stages. */
    std::vector<std::unique_ptr<Stage>> stages_;
};
}
