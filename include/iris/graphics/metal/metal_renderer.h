////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "graphics/material_manager.h"
#include "graphics/metal/metal_constant_buffer.h"
#include "graphics/metal/metal_material.h"
#include "graphics/metal/metal_render_target.h"
#include "graphics/metal/metal_texture.h"
#include "graphics/render_command.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_pipeline.h"
#include "graphics/renderer.h"
#include "graphics/texture_manager.h"

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
     * @param texture_manager
     *   Texture manager object.
     *
     * @param material_manager
     *   Material manager object.
     *
     * @param width
     *   Width of window being rendered to.
     *
     * @param height
     *   Height of window being rendered to.
     */
    MetalRenderer(
        TextureManager &texture_manager,
        MaterialManager &material_manager,
        std::uint32_t width,
        std::uint32_t height);

    /**
     * Destructor, will block until all inflight frames have finished rendering.
     */
    ~MetalRenderer() override;

  protected:
    /**
     * Render specific method to set the render pipeline.
     *
     * @param build_queue
     *   Function to build queue.
     */
    void do_set_render_pipeline(std::function<void()> build_queue) override;

    // handlers for the supported RenderCommandTypes

    void pre_render() override;
    void execute_pass_start(RenderCommand &command) override;
    void execute_draw(RenderCommand &command) override;
    void execute_pass_end(RenderCommand &command) override;
    void execute_present(RenderCommand &command) override;
    void post_render() override;

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

        /** Map of bone data buffers to render entities. */
        std::unordered_map<const RenderEntity *, std::unique_ptr<MetalConstantBuffer>> bone_data;

        /** Map of model data buffers to render entities. */
        std::unordered_map<const RenderEntity *, std::unique_ptr<MetalConstantBuffer>> model_data;

        /** Map of light data buffers to lights. */
        std::unordered_map<const Light *, std::unique_ptr<MetalConstantBuffer>> light_data;

        std::unordered_map<const Camera *, std::unique_ptr<MetalConstantBuffer>> camera_data;

        std::unordered_map<const Material *, std::unique_ptr<MetalConstantBuffer>> property_data;
    };

    /** Texture manager object. */
    TextureManager &texture_manager_;

    /** Current command queue. */
    id<MTLCommandQueue> command_queue_;

    /** Default descriptor for all render passes. */
    MTLRenderPassDescriptor *single_pass_descriptor_;

    MTLRenderPassDescriptor *multi_pass_descriptor_;

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
    std::unordered_map<const RenderTarget *, id<MTLRenderCommandEncoder>> render_encoders_;

    /** The depth buffer for the default frame. */
    std::unique_ptr<MetalTexture> default_depth_buffer_;

    /** Map of instance data buffers to render entities.  */
    std::unordered_map<const RenderEntity *, std::unique_ptr<MetalConstantBuffer>> instance_data_;

    /** Buffer for bindless texture table. */
    std::unique_ptr<MetalConstantBuffer> texture_table_;

    /** Buffer for bindless cube map table. */
    std::unique_ptr<MetalConstantBuffer> cube_map_table_;

    /** Buffer for bindless sampler table. */
    std::unique_ptr<MetalConstantBuffer> sampler_table_;

    /** Collection of resources that need to be made resident before rendering. */
    std::vector<id<MTLResource>> resident_resources_;
};

}
