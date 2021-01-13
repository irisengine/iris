#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "core/camera.h"
#include "core/camera_type.h"
#include "core/vector3.h"
#include "graphics/pipeline.h"
#include "graphics/render_target.h"

namespace iris
{

/**
 * Class for performing rendering. This class defines the interface for a
 * render system, there are API specific implementations which are selected at
 * compile time.
 */
class RenderSystem
{
  public:
    /**
     * Create a new rendering system.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     *
     * @param screen_target
     *   Render target to present to the window after rendering.
     */
    RenderSystem(float width, float height, RenderTarget *screen_target);

    ~RenderSystem();
    RenderSystem(RenderSystem &&);
    RenderSystem &operator=(RenderSystem &&);

    /**
     * Render a Pipeline. Afterwards the default screen target is displayed.
     *
     * @param pipeline
     *   Pipeline to execute.
     */
    void render(const Pipeline &pipeline);

  private:
    /** Render target for screen. */
    RenderTarget *screen_target_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
