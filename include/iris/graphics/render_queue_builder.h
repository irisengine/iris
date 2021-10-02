#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "graphics/lights/light_type.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * This class provides a generic way of building a render queue (i.e. a
 * collection of RenderCommand objects) from a collection of RenderPass objects.
 *
 * It is decoupled from any graphics API by requiring callbacks for graphics
 * specific objects.
 */
class RenderQueueBuilder
{
  public:
    // aliases for callbacks
    using CreateMaterialCallback = std::function<
        Material
            *(RenderGraph *, RenderEntity *, const RenderTarget *, LightType)>;
    using CreateRenderTargetCallback =
        std::function<RenderTarget *(std::uint32_t, std::uint32_t)>;

    /**
     * Construct a new RenderQueueBuilder
     *
     * @param create_material_callback
     *   Callback fro creating a Material object.
     *
     * @param create_render_target_callback
     *   Callback for creating a RenderTarget object.
     */
    RenderQueueBuilder(
        CreateMaterialCallback create_material_callback,
        CreateRenderTargetCallback create_render_target_callback);

    /**
     * Build a render queue (a collection of RenderCommand objects) from a
     * collection of RenderPass objects.
     *
     * @param render_passes
     *   RenderPass objects to create a render queue from.
     *
     * @returns
     *   Collection of RenderCommand objects which when executed will render the
     *   supplied passes.
     */
    std::vector<RenderCommand> build(
        std::vector<RenderPass> &render_passes) const;

  private:
    /**   Callback fro creating a Material object. */
    CreateMaterialCallback create_material_callback_;

    /**   Callback for creating a RenderTarget object. */
    CreateRenderTargetCallback create_render_target_callback_;
};

}
