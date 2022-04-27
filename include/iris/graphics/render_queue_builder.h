////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

#include "core/camera.h"
#include "graphics/lights/light_type.h"
#include "graphics/post_processing_description.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_pass.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"
#include "graphics/scene.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * This class provides a generic way of building a render queue (i.e. a collection of RenderCommand objects) from a
 * collection of RenderPass objects.
 *
 * It is decoupled from any graphics API by requiring callbacks for graphics specific objects.
 *
 * Note that as part of the building process it may create additional render passes. The data for these will be stored
 * internally so this object must live for as long as the queue of render commands is being executed.
 *
 * There is a lot of messy logic in building up the various passes required for lighting and post processing effects. We
 * push all that down to this class so at least its in one place and independent of any actual rendering code.
 *
 * As a worked example, say we want to build a command queue for the following scene:
 *  - Ambient light
 *  - Directional light (with shadow casting)
 *  - Postprocessing effects:
 *      - SSAO
 *      - Colour Adjustment
 *
 *    +-------+
 *    | Scene |
 *    +-------+
 *        |
 *        |  +-----------------+
 *        +->| Shadow map pass |----> Shadow map -----------------.
 *        |  +-----------------+                                  |
 *        |                                                       |
 *        |  +---------------+                                    |
 *        +->| SSAO pre pass |--+--> Ambient colour ----------.   |
 *        |  +---------------+  |                             |   |
 *        |                     +--> Screen space normals ----+   |
 *        |                     |                             |   |
 *        |                     '--> Screen space positions --+   |
 *        |                                                   |   |
 *        |   .-----------------------------------------------'   |
 *        |   |                                                   |
 *        |   |   +-----------+                                   |
 *        |   '-->| SSAO pass |----> Ambient light + occlusion ---|-----.
 *        |       +-----------+                                   |     |
 *        |                                                       |     |
 *        | .-----------------------------------------------------'     |
 *        | |                                                   .-------'
 *        | |                                                   |
 *        | |   +------------------------+        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *        '-+-->| Directional light pass |------> # Light pass blended target #----.
 *              +------------------------+        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    |
 *                                                                                 |
 *        .------------------------------------------------------------------------'
 *        |     +--------------------+                 ~~~~~~~~~~~~~~~~
 *        '---->| Colour adjust pass |----------------># Final target #
 *              +--------------------+                 ~~~~~~~~~~~~~~~~
 *
 * This would then get turned into render commands
 */
class RenderQueueBuilder
{
  public:
    // aliases for callbacks
    using CreateMaterialCallback =
        std::function<Material *(RenderGraph *, RenderEntity *, const RenderTarget *, LightType, bool, bool)>;
    using CreateRenderTargetCallback = std::function<RenderTarget *(std::uint32_t, std::uint32_t)>;
    using CreateHybriRenderTargetCallback = std::function<RenderTarget *(const RenderTarget *, const RenderTarget *)>;

    /**
     * Create a new RenderQueueBuilder.
     *
     * @param width
     *   The width of the final render target.
     *
     * @param height
     *   The height of the final render target.
     *
     * @param create_material_callback
     *   Callback for creating a material object.
     *
     * @param create_render_target_callback
     *   Callback for creating a render target object.
     *
     * @param create_rhybrind_ender_target_callback
     *   Callback for creating a hybrid render target i.e. a render target which combines the colour and depth texture
     * from two other render targets.
     */
    RenderQueueBuilder(
        std::uint32_t width,
        std::uint32_t height,
        CreateMaterialCallback create_material_callback,
        CreateRenderTargetCallback create_render_target_callback,
        CreateHybriRenderTargetCallback create_hybrid_render_target_callback);

    /**
     * Build a command queue from a collection of render passes.
     *
     * @param render_passes
     *   Render passes to build queue from, this will get mutated as new passes are added.
     *
     * @returns
     *   Collection of render commands which when executed will render the provided passes.
     */
    std::vector<RenderCommand> build(std::vector<RenderPass> &render_passes);

  private:
    /**
     * Add a new pass to collection of passes. This creates a new scene, camera and render target as well as using a
     * callback to allow the caller to set the render graph for the scene.
     */
    const RenderTarget *add_pass(
        std::vector<RenderPass> &render_passes,
        RenderPass **prev,
        std::function<void(iris::RenderGraph *, const RenderTarget *)> create_render_graph_callback);

    void add_post_processing_passes(
        const std::vector<RenderPass> &initial_passes,
        std::vector<RenderPass> &render_passes);

    struct PassData
    {
        Scene scene;
        Camera camera;
    };

    std::uint32_t width_;
    std::uint32_t height_;
    CreateMaterialCallback create_material_callback_;
    CreateRenderTargetCallback create_render_target_callback_;
    CreateHybriRenderTargetCallback create_hybrid_render_target_callback_;
    std::deque<PassData> pass_data_;
};

}
