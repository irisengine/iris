////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <vector>

#include "core/camera.h"
#include "graphics/lights/light_type.h"
#include "graphics/render_command.h"
#include "graphics/render_pass.h"
#include "graphics/render_pipeline.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{

/**
 * Interface for a Renderer - a class that executes a collection of RenderPass
 * objects.
 */
class Renderer
{
  public:
    Renderer();
    virtual ~Renderer() = default;

    /**
     * Render the current RenderPass objects.
     */
    virtual void render();

    /**
     * Set the render pipeline to execute with render().
     *
     * @param render_pipeline
     *   Pipeline to execute.
     */
    void set_render_pipeline(std::unique_ptr<RenderPipeline> render_pipeline);

  protected:
    /**
     * Implementers should use this method to perform any engine specific tasks before/after building the render queue.
     *
     * @param build_queue
     *   This function builds the queue, it *must* be called.
     */
    virtual void do_set_render_pipeline(std::function<void()> build_queue) = 0;

    // these functions provide implementors a chance to handle each
    // RenderCommandType, where each function below corresponds to one of the
    // enum types - with the addition of pre_render and post_render which get
    // called before and after each frame
    // defaults are all no-ops so implementations only need to override the
    // ones needed for their graphics api

    virtual void pre_render();
    virtual void execute_pass_start(RenderCommand &command);
    virtual void execute_draw(RenderCommand &command);
    virtual void execute_pass_end(RenderCommand &command);
    virtual void execute_present(RenderCommand &command);
    virtual void post_render();

    /** The queue of RenderCommand objects created from the current RenderPass objects. */
    std::vector<RenderCommand> render_queue_;

    /** Pipeline to execute with render(). */
    std::unique_ptr<RenderPipeline> render_pipeline_;
};

}
