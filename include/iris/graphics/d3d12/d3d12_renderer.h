////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <deque>
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
#include "graphics/d3d12/d3d12_cube_map.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_root_signature.h"
#include "graphics/d3d12/d3d12_structured_buffer.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/material_cache.h"
#include "graphics/render_pipeline.h"
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
    D3D12Renderer(HWND window, std::uint32_t width, std::uint32_t height, std::uint32_t initial_screen_scale);

    /**
     * Destructor, will block until all inflight frames have finished rendering.
     */
    ~D3D12Renderer() override;

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

  private:
    /**
     * Internal struct encapsulating data needed for a frame.
     */
    struct Frame
    {
        Frame(
            std::uint32_t frame,
            Microsoft::WRL::ComPtr<ID3D12Resource> buffer,
            D3D12DescriptorHandle render_target,
            std::unique_ptr<D3D12Texture> depth_buffer,
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator)
            : frame_id(frame)
            , buffer(buffer)
            , render_target(render_target)
            , depth_buffer(std::move(depth_buffer))
            , command_allocator(command_allocator)
            , fence_value(0u)
            , bone_data_buffers()
            , model_data_buffers()
            , light_data_buffers()
            , camera_data_buffers()
        {
        }

        /** Unique id for frame.*/
        std::uint32_t frame_id;

        /** D3D12 back buffer to render to and present. */
        Microsoft::WRL::ComPtr<ID3D12Resource> buffer;

        /** render target view for buffer. */
        D3D12DescriptorHandle render_target;

        /** Depth buffer for frame. */
        std::unique_ptr<D3D12Texture> depth_buffer;

        /** Command allocate for frame. */
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator;

        /** Current fence value for frame. */
        std::uint64_t fence_value;

        /** Cache of bone data buffers for render entities in this frame. */
        std::unordered_map<const RenderEntity *, std::unique_ptr<D3D12ConstantBuffer>> bone_data_buffers;

        /** Cache of model data buffers for render entities in this frame. */
        std::unordered_map<const RenderEntity *, std::unique_ptr<D3D12StructuredBuffer>> model_data_buffers;

        /** Cache of data buffers for lights in this frame. */
        std::unordered_map<const Light *, std::unique_ptr<D3D12ConstantBuffer>> light_data_buffers;

        /** Cache of data buffers for cameras in this frame. */
        std::unordered_map<const Camera *, std::unique_ptr<D3D12ConstantBuffer>> camera_data_buffers;
    };

    /** Width of window to present to. */
    std::uint32_t width_;

    /** Height of window to present to. */
    std::uint32_t height_;

    /** Collection of frames for triple buffering. */
    std::vector<Frame> frames_;

    /** Index of current frame to render to. */
    std::uint32_t frame_index_;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

    AutoRelease<HANDLE, nullptr> fence_event_;

    /** Single command queue for all frames. */
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_;

    /** Single command list for all frames. */
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list_;

    /** Swap chain for triple buffering. */
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swap_chain_;

    /** Viewport for window. */
    CD3DX12_VIEWPORT viewport_;

    /** Scissor rect for window. */
    CD3DX12_RECT scissor_rect_;

    /** Collection of textures that have been uploaded. */
    std::set<const D3D12Texture *> uploaded_textures_;

    /** Collection of CubeMaps that have been uploaded. */
    std::set<const D3D12CubeMap *> uploaded_cube_maps_;

    /** Cache of instance model data buffers for instanced entities in the current scene. */
    std::unordered_map<const RenderEntity *, std::unique_ptr<D3D12StructuredBuffer>> instance_data_buffers_;

    /** Descriptor handle to a global texture table (for bindless). */
    D3D12DescriptorHandle texture_table_;

    /** Descriptor handle to a global cube map table (for bindless). */
    D3D12DescriptorHandle cube_map_table_;

    /** Descriptor handle to a global sampler table (for bindless). */
    D3D12DescriptorHandle sampler_table_;
};
}
