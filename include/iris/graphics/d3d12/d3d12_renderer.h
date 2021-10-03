#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/auto_release.h"
#include "graphics/d3d12/d3d12_constant_buffer.h"
#include "graphics/d3d12/d3d12_constant_buffer_pool.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_material.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"

namespace iris
{

/**
 * Implementation of Renderer for d3d12.
 *
 * This Renderer uses triple buffering to allow for greatest rendering
 * throughput. A frame if defined as all rendering passes that occur when
 * render() is called. This class uses a circular buffer of three frames. When
 * all the CPU processing of a frame is complete it is submitted to the GPU. At
 * this point the CPU is free to proceed to the next frame whilst the GPU works
 * asynchronously.
 */
class D3D12Renderer : public Renderer
{
  public:
    /**
     * Construct a new D3D12Renderer.
     *
     * @param window
     *   The window to present to.
     *
     * @param width
     *   Width of window being rendered to.
     *
     * @param height
     *   Height of window being rendered to.
     *
     * @param initial_screen_scale
     *   The natural scale of the screen with window is currently on.
     */
    D3D12Renderer(
        HWND window,
        std::uint32_t width,
        std::uint32_t height,
        std::uint32_t initial_screen_scale);

    /**
     * Destructor, will block until all inflight frames have finished rendering.
     */
    ~D3D12Renderer() override;

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
    void execute_upload_texture(RenderCommand &command) override;
    void execute_pass_start(RenderCommand &command) override;
    void execute_draw(RenderCommand &command) override;
    void execute_pass_end(RenderCommand &command) override;
    void execute_present(RenderCommand &command) override;

  private:
    /**
     * Internal struct encapsulating data needed for a frame.
     */
    struct Frame
    {
        Frame(
            Microsoft::WRL::ComPtr<ID3D12Resource> buffer,
            D3D12DescriptorHandle render_target,
            std::unique_ptr<D3D12Texture> depth_buffer,
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator,
            Microsoft::WRL::ComPtr<ID3D12Fence> fence,
            HANDLE fence_event)
            : buffer(buffer)
            , render_target(render_target)
            , depth_buffer(std::move(depth_buffer))
            , command_allocator(command_allocator)
            , fence(fence)
            , fence_event(fence_event, ::CloseHandle)
        {
        }

        /** D3D12 back buffer to render to and present. */
        Microsoft::WRL::ComPtr<ID3D12Resource> buffer;

        /** render target view for buffer. */
        D3D12DescriptorHandle render_target;

        /** Depth buffer for frame. */
        std::unique_ptr<D3D12Texture> depth_buffer;

        /** Command allocate for frame. */
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator;

        /** Fence for signaling frame completion. */
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;

        /**
         * Event to signal frame completion, when event is set then frame is
         * safe to use.
         */
        AutoRelease<HANDLE, nullptr> fence_event;

        /** Map of RenderCommand objects to constant data buffer pools. */
        std::unordered_map<const RenderCommand *, D3D12ConstantBufferPool>
            constant_data_buffers;
    };

    /** Width of window to present to. */
    std::uint32_t width_;

    /** Height of window to present to. */
    std::uint32_t height_;

    /** Collection of frames for triple buffering. */
    std::vector<Frame> frames_;

    /** Index of current frame to render to. */
    std::uint32_t frame_index_;

    /** Single command queue for all frames. */
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_;

    /** Single command list for all frames. */
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list_;

    /** Swap chain for triple buffering. */
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swap_chain_;

    /** Special null buffer to use as padding in table descriptors. */
    std::unique_ptr<D3D12ConstantBuffer> null_buffer_;

    /** Viewport for window. */
    CD3DX12_VIEWPORT viewport_;

    /** Scissor rect for window. */
    CD3DX12_RECT scissor_rect_;

    /** Collection of created RenderTarget objects. */
    std::vector<std::unique_ptr<RenderTarget>> render_targets_;

    /** This collection stores materials per light type per render graph. */
    std::unordered_map<
        RenderGraph *,
        std::unordered_map<LightType, std::unique_ptr<D3D12Material>>>
        materials_;

    /** Collection of textures that have been uploaded. */
    std::set<const D3D12Texture *> uploaded_;
};
}
