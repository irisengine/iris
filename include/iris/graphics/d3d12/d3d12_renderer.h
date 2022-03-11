////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
#include "graphics/d3d12/d3d12_cube_map.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_material.h"
#include "graphics/d3d12/d3d12_root_signature.h"
#include "graphics/d3d12/d3d12_structured_buffer.h"
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
    D3D12Renderer(HWND window, std::uint32_t width, std::uint32_t height, std::uint32_t initial_screen_scale);

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
    void set_render_passes(const std::vector<RenderPass> &render_passes) override;

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
    RenderTarget *create_render_target(std::uint32_t width, std::uint32_t height) override;

  protected:
    // handlers for the supported RenderCommandTypes

    void pre_render() override;
    void execute_pass_start(RenderCommand &command) override;
    void execute_draw(RenderCommand &command) override;
    void execute_pass_end(RenderCommand &command) override;
    void execute_present(RenderCommand &command) override;

  private:
    /**
     * Custom equality functor for our unordered maps of render graphs. This is because we want no collisions, any graph
     * hashing the same should be considered equal.
     */
    struct RenderGraphPtrEqual
    {
        bool operator()(const RenderGraph *a, const RenderGraph *b) const
        {
            return std::hash<RenderGraph *>{}(a) == std::hash<RenderGraph *>{}(b);
        }
    };

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

    /** Collection of created RenderTarget objects. */
    std::vector<std::unique_ptr<RenderTarget>> render_targets_;

    /** This collection stores materials per light type per render graph. */
    std::unordered_map<
        RenderGraph *,
        std::unordered_map<LightType, std::unique_ptr<D3D12Material>>,
        std::hash<RenderGraph *>,
        RenderGraphPtrEqual>
        materials_;

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

    /** Root signature for materials. */
    D3D12RootSignature<
        ConstantBufferViewParameter<0u, 0u, D3D12_SHADER_VISIBILITY_ALL>,
        ConstantBufferViewParameter<1u, 0u, D3D12_SHADER_VISIBILITY_ALL>,
        ConstantBufferViewParameter<2u, 0u, D3D12_SHADER_VISIBILITY_ALL>,
        ConstantParameter<3u, 0u, D3D12_SHADER_VISIBILITY_ALL>,
        ShaderResourceViewParameter<0u, 0u, D3D12_SHADER_VISIBILITY_ALL>,
        TableParameter<D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0u, 1u, D3D12_SHADER_VISIBILITY_PIXEL>,
        TableParameter<D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0u, 2u, D3D12_SHADER_VISIBILITY_PIXEL>>
        root_signature_;
};
}
