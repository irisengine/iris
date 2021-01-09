#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "core/camera.h"
#include "core/camera_type.h"
#include "core/vector3.h"
#include "graphics/pipeline.h"

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
     */
    RenderSystem(float width, float height);

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
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
