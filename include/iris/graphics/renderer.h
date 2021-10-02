#pragma once

#include "core/camera.h"
#include "graphics/lights/light_type.h"
#include "graphics/render_command.h"
#include "graphics/render_pass.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

#include <vector>

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
     * Set the render passes. These will be executed when render() is called.
     *
     * @param render_passes
     *   Collection of RenderPass objects to render.
     */
    virtual void set_render_passes(
        const std::vector<RenderPass> &render_passes) = 0;

    /**
     * Create a RenderTarget with custom dimensions.
     *
     * @param width
     *   Width of render target.
     *
     * @param height
     *   Height of render target.
     *
     * @returns
     *   RenderTarget.
     */
    virtual RenderTarget *create_render_target(
        std::uint32_t width,
        std::uint32_t height) = 0;

  protected:
    // these functions provide implementors a chance to handle each
    // RenderCommandType, where each function below corresponds to one of the
    // enum types - with the addition of pre_render and post_render which get
    // called before and after each frame
    // defaults are all no-ops so implementations only need to override the
    // ones needed for their graphics api

    virtual void pre_render();
    virtual void execute_upload_texture(RenderCommand &command);
    virtual void execute_pass_start(RenderCommand &command);
    virtual void execute_draw(RenderCommand &command);
    virtual void execute_pass_end(RenderCommand &command);
    virtual void execute_present(RenderCommand &command);
    virtual void post_render();

    /** The collection of RenderPass objects to be rendered. */
    std::vector<RenderPass> render_passes_;

    /**
     * The queue of RenderCommand objects created from the current RenderPass
     * objects.
     * */
    std::vector<RenderCommand> render_queue_;

    /** Scene for the post processing step. */
    std::unique_ptr<Scene> post_processing_scene_;

    /** RenderTarget for the post processing step. */
    RenderTarget *post_processing_target_;

    /** Camera for the post processing step. */
    std::unique_ptr<Camera> post_processing_camera_;
};

}
