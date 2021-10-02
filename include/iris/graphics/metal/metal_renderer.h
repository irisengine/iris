#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "core/root.h"
#include "graphics/metal/metal_constant_buffer.h"
#include "graphics/metal/metal_material.h"
#include "graphics/metal/metal_render_target.h"
#include "graphics/render_command.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/renderer.h"
#include "graphics/window_manager.h"

namespace iris
{

/**
 * Implementation of Renderer for metal.
 *
 * This Renderer uses triple buffering to allow for greatest rendering
 * throughput. A frame if defined as all rendering passes that occur when
 * render() is called. This class uses a circular buffer of three frames. When
 * all the CPU processing of a frame is complete it is submitted to the GPU. At
 * this point the CPU is free to proceed to the next frame whilst the GPU works
 * asynchronously.
 */
class MetalRenderer : public Renderer
{
  public:
    /**
     * Construct a new MetalRenderer.
     *
     * @param width
     *   Width of window being rendered to.
     *
     * @param height
     *   Height of window being rendered to.
     */
    MetalRenderer(std::uint32_t width, std::uint32_t height);

    /**
     * Destructor, will block until all inflight frames have finished rendering.
     */
    ~MetalRenderer() override;

    /**
     * Set the render passes. These will be executed when render() is called.
     *
     * @param render_passes
     *   Collection of RenderPass objects to render.
     */
    void set_render_passes(
        const std::vector<RenderPass> &render_passes) override;

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
    RenderTarget *create_render_target(
        std::uint32_t width,
        std::uint32_t height) override;

  protected:
    // handlers for the supported RenderCommandTypes

    void pre_render() override;
    void execute_draw(RenderCommand &command) override;
    void execute_pass_end(RenderCommand &command) override;
    void execute_present(RenderCommand &command) override;
    void post_render() override;

  private:
    // helper aliases to try and simplify the verbose types
    using LightMaterialMap =
        std::unordered_map<LightType, std::unique_ptr<MetalMaterial>>;

    /**
     * Internal struct encapsulating data needed for a frame.
     */
    struct Frame
    {
        /**
         * Lock to ensure GPU has finished executing frame before we try and
         * write to it again.
         * */
        std::mutex lock;

        /**
         * Map of render commands to constant buffers - this ensures each draw
         * command gets its own buffer.
         */
        std::unordered_map<const RenderCommand *, MetalConstantBuffer>
            constant_data_buffers;
    };

    /** Width of window to render to. */
    std::uint32_t width_;

    /** Height of window to render to. */
    std::uint32_t height_;

    /** Current command queue. */
    id<MTLCommandQueue> command_queue_;

    /** Default descriptor for all render passes. */
    MTLRenderPassDescriptor *descriptor_;

    /** Current metal drawable. */
    id<CAMetalDrawable> drawable_;

    /** Current command buffer. */
    id<MTLCommandBuffer> command_buffer_;

    /** Default state for depth buffers. */
    id<MTLDepthStencilState> depth_stencil_state_;

    /** Current render encoder. */
    id<MTLRenderCommandEncoder> render_encoder_;

    /** Current frame number. */
    std::size_t current_frame_;

    /** Triple buffered frames. */
    std::array<Frame, 3u> frames_;

    /** Map of targets to render encoders. */
    std::unordered_map<const RenderTarget *, id<MTLRenderCommandEncoder>>
        render_encoders_;

    /** Collection of created RenderTarget objects. */
    std::vector<std::unique_ptr<MetalRenderTarget>> render_targets_;

    /** This collection stores materials per light type per render graph. */
    std::unordered_map<const RenderGraph *, LightMaterialMap> materials_;

    /** The depth buffer for the default frame. */
    std::unique_ptr<MetalTexture> default_depth_buffer_;

    /** Default sampler for shadow maps. */
    id<MTLSamplerState> shadow_sampler_;
};

}
