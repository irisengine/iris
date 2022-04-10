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

class RenderQueueBuilder
{
  public:
    // aliases for callbacks
    using CreateMaterialCallback =
        std::function<Material *(RenderGraph *, RenderEntity *, const RenderTarget *, LightType, bool, bool)>;
    using CreateRenderTargetCallback = std::function<RenderTarget *(std::uint32_t, std::uint32_t)>;
    using CreateHybriRenderTargetCallback = std::function<RenderTarget *(const RenderTarget *, const RenderTarget *)>;

    RenderQueueBuilder(
        std::uint32_t width,
        std::uint32_t height,
        CreateMaterialCallback create_material_callback,
        CreateRenderTargetCallback create_render_target_callback,
        CreateHybriRenderTargetCallback create_hybrid_render_target_callback);

    std::vector<RenderCommand> build(std::vector<RenderPass> &render_passes);

  private:
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
