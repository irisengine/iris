////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_renderer.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <dxgi1_6.h>
#include <hidusage.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "core/root.h"
#include "graphics/constant_buffer_writer.h"
#include "graphics/d3d12/d3d12_constant_buffer.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/d3d12/d3d12_mesh.h"
#include "graphics/d3d12/d3d12_render_target.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/mesh_manager.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/post_processing_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_queue_builder.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"
#include "log/log.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")

namespace
{

// this matrix is used to translate projection matrices from engine NDC to
// metal NDC
static const iris::Matrix4 directx_translate{
    {1.0f,
     0.0f,
     0.0f,
     0.0f,
     0.0f,
     1.0f,
     0.0f,
     0.0f,
     0.0f,
     0.0f,
     0.5f,
     0.5f,
     0.0f,
     0.0f,
     0.0f,
     1.0f}};

/**
 * Helper function to write vertex data into a constant buffer.
 *
 * @param constant_buffer
 *   D3D12ConstantBuffer object to write to.
 *
 * @param camera
 *   Camera for currently rendered scene.
 *
 * @param entity
 *   Entity being rendered.
 *
 * @param light_data
 *   Light for current render pass.
 */
void write_vertex_data_constant_buffer(
    iris::D3D12ConstantBuffer &constant_buffer,
    const iris::Camera *camera,
    const iris::RenderEntity *entity,
    const iris::Light *light)
{
    iris::ConstantBufferWriter writer(constant_buffer);

    writer.write(directx_translate * camera->projection());
    writer.write(camera->view());
    writer.write(camera->position());
    writer.write(0.0f);
    writer.write(entity->transform());
    writer.write(entity->normal_transform());

    const auto &bones = entity->skeleton().transforms();
    writer.write(bones);
    writer.advance((100u - bones.size()) * sizeof(iris::Matrix4));

    writer.write(light->colour_data());
    writer.write(light->world_space_data());
    writer.write(light->attenuation_data());
}

/**
 * Helper function to write light specific data to a constant buffer. Note that
 * this is for additional light data not written to the main vertex constant
 * buffer.
 *
 * @param constant_buffer
 *   D3D12ConstantBuffer object to write to.
 *
 * @param light
 *   Light to get data from.
 */
void write_directional_light_data_constant_buffer(
    iris::D3D12ConstantBuffer &constant_buffer,
    const iris::Light *light)
{
    const auto *d3d12_light =
        static_cast<const iris::DirectionalLight *>(light);

    iris::ConstantBufferWriter writer(constant_buffer);
    writer.write(directx_translate * d3d12_light->shadow_camera().projection());
    writer.write(d3d12_light->shadow_camera().view());
}

/**
 * Helper function to copy a descriptor handle.
 *
 * @param dest
 *   Where to write the descriptor to. This is advanced by descriptor_size.
 *
 * @param source
 *   Source handle to copy from.
 *
 * @param descriptor_size
 *   Size of the descriptor handle.
 */
void copy_descriptor(
    D3D12_CPU_DESCRIPTOR_HANDLE &dest,
    const iris::D3D12DescriptorHandle &source,
    std::size_t descriptor_size)
{
    auto *device = iris::D3D12Context::device();

    device->CopyDescriptorsSimple(
        1u, dest, source.cpu_handle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    dest.ptr += descriptor_size;
}

/**
 * Build the table descriptor.
 *
 * @param table_descriptor
 *   The start of the table to write descriptors to.
 *
 * @param descriptor_size
 *   Size of the descriptor handle.
 *
 * @param vertex_constant_buffer
 *   D3D12DescriptorHandle for vertex data.
 *
 * @param light_constant_buffer
 *   D3D12DescriptorHandle for additional light data.
 *
 * @param shadow_map
 *   D3D12DescriptorHandle for shadow map texture.
 *
 * @param textures
 *   Collection of textures for the render pass.
 */
void build_table_descriptor(
    D3D12_CPU_DESCRIPTOR_HANDLE &table_descriptor,
    std::size_t descriptor_size,
    const iris::D3D12DescriptorHandle &vertex_constant_buffer,
    const iris::D3D12DescriptorHandle &light_constant_buffer,
    const iris::D3D12DescriptorHandle &shadow_map,
    const std::vector<iris::Texture *> &textures)
{
    copy_descriptor(table_descriptor, vertex_constant_buffer, descriptor_size);
    copy_descriptor(table_descriptor, light_constant_buffer, descriptor_size);
    copy_descriptor(table_descriptor, shadow_map, descriptor_size);

    for (auto *texture : textures)
    {
        auto *d3d12_tex = static_cast<iris::D3D12Texture *>(texture);
        copy_descriptor(table_descriptor, d3d12_tex->handle(), descriptor_size);
    }
}

}

namespace iris
{

D3D12Renderer::D3D12Renderer(
    HWND window,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t initial_screen_scale)
    : width_(width)
    , height_(height)
    , frames_()
    , frame_index_(0u)
    , command_queue_(nullptr)
    , command_list_(nullptr)
    , swap_chain_(nullptr)
    , null_buffer_(nullptr)
    , viewport_()
    , scissor_rect_()
    , render_targets_()
    , materials_()
    , uploaded_()

{
    // we will use triple buffering
    const auto num_frames = 3u;

    D3D12_COMMAND_QUEUE_DESC desc = {0};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    auto *device = D3D12Context::device();
    auto *dxgi_factory = D3D12Context::dxgi_factory();

    // create command queue
    ensure(
        device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue_)) ==
            S_OK,
        "could not create command queue");

    // build swap chain description
    DXGI_SWAP_CHAIN_DESC1 swap_chain_descriptor = {0};
    swap_chain_descriptor.Width = width_ * initial_screen_scale;
    swap_chain_descriptor.Height = height_ * initial_screen_scale;
    swap_chain_descriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_descriptor.Stereo = FALSE;
    swap_chain_descriptor.SampleDesc = {1, 0};
    swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_descriptor.BufferCount = num_frames;
    swap_chain_descriptor.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_descriptor.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_descriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    // create swap chain for window
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain_tmp = nullptr;
    ensure(
        dxgi_factory->CreateSwapChainForHwnd(
            command_queue_.Get(),
            window,
            &swap_chain_descriptor,
            nullptr,
            nullptr,
            &swap_chain_tmp) == S_OK,
        "could not create swap chain");

    // cast to type we want to use
    ensure(
        swap_chain_tmp.As(&swap_chain_) == S_OK, "could not cast swap chain");

    // get initial frame index
    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    // build our frame buffers
    for (auto i = 0u; i < num_frames; ++i)
    {
        // get a back buffer
        Microsoft::WRL::ComPtr<ID3D12Resource> frame = nullptr;
        ensure(
            swap_chain_->GetBuffer(i, IID_PPV_ARGS(&frame)) == S_OK,
            "could not get back buffer");

        static int counter = 0;
        std::wstringstream strm{};
        strm << L"frame_" << counter++;
        const auto name = strm.str();
        frame->SetName(name.c_str());

        // create a static descriptor handle for the render target
        auto rtv_handle = D3D12DescriptorManager::cpu_allocator(
                              D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
                              .allocate_static();

        device->CreateRenderTargetView(
            frame.Get(), nullptr, rtv_handle.cpu_handle());

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator =
            nullptr;

        // create command allocator, we use one per frame but have a single
        // command queue
        ensure(
            device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&command_allocator)) == S_OK,
            "could not create command allocator");

        // create a fence, used to signal when gpu has completed a frame
        Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
        ensure(
            device->CreateFence(
                0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) == S_OK,
            "could not create fence");

        frames_.emplace_back(
            frame,
            rtv_handle,
            std::make_unique<D3D12Texture>(
                DataBuffer{},
                width_ * initial_screen_scale,
                height_ * initial_screen_scale,
                TextureUsage::DEPTH),
            command_allocator,
            fence,
            ::CreateEvent(NULL, FALSE, TRUE, NULL));
    }

    ensure(
        device->CreateCommandList(
            0u,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            frames_[frame_index_].command_allocator.Get(),
            nullptr,
            IID_PPV_ARGS(&command_list_)) == S_OK,
        "could not create command list");

    // create a single null buffer, used for padding buffer arrays
    null_buffer_ = std::make_unique<D3D12ConstantBuffer>();

    // close the list so we can start recording to it
    command_list_->Close();
}

D3D12Renderer::~D3D12Renderer()
{
    // build a collection of all frame events
    std::vector<HANDLE> wait_handles{};

    for (const auto &frame : frames_)
    {
        wait_handles.emplace_back(frame.fence_event);
    }

    // we cannot destruct whilst a frame is being rendered, so we wait for all
    // frames to signal they are done
    ::WaitForMultipleObjects(
        static_cast<DWORD>(wait_handles.size()),
        wait_handles.data(),
        TRUE,
        INFINITE);
}

void D3D12Renderer::set_render_passes(
    const std::vector<RenderPass> &render_passes)
{
    render_passes_ = render_passes;

    // add a post processing pass

    // find the pass which renders to the screen
    auto final_pass = std::find_if(
        std::begin(render_passes_),
        std::end(render_passes_),
        [](const RenderPass &pass) { return pass.render_target == nullptr; });

    ensure(final_pass != std::cend(render_passes_), "no final pass");

    // deferred creating of render target to ensure this class is full
    // constructed
    if (post_processing_target_ == nullptr)
    {
        post_processing_target_ = create_render_target(width_, height_);
        post_processing_camera_ =
            std::make_unique<Camera>(CameraType::ORTHOGRAPHIC, width_, height_);
    }

    post_processing_scene_ = std::make_unique<Scene>();

    // create a full screen quad which renders the final stage with the post
    // processing node
    auto *rg = post_processing_scene_->create_render_graph();
    rg->set_render_node<PostProcessingNode>(
        rg->create<TextureNode>(post_processing_target_->colour_texture()));
    post_processing_scene_->create_entity(
        rg,
        Root::mesh_manager().sprite({}),
        Transform(
            {},
            {},
            {static_cast<float>(width_), static_cast<float>(height_), 1.0}));

    // wire up this pass
    final_pass->render_target = post_processing_target_;
    render_passes_.emplace_back(
        post_processing_scene_.get(), post_processing_camera_.get(), nullptr);

    // build the render queue from the provided passes

    RenderQueueBuilder queue_builder(
        [this](
            RenderGraph *render_graph,
            RenderEntity *render_entity,
            const RenderTarget *target,
            LightType light_type) {
            if (materials_.count(render_graph) == 0u ||
                materials_[render_graph].count(light_type) == 0u)
            {
                materials_[render_graph][light_type] =
                    std::make_unique<D3D12Material>(
                        render_graph,
                        static_cast<D3D12Mesh *>(render_entity->mesh())
                            ->input_descriptors(),
                        render_entity->primitive_type(),
                        light_type,
                        target == nullptr);
            }

            return materials_[render_graph][light_type].get();
        },
        [this](std::uint32_t width, std::uint32_t height) {
            return create_render_target(width, height);
        });
    render_queue_ = queue_builder.build(render_passes_);

    // clear all constant data buffers
    for (auto &frame : frames_)
    {
        frame.constant_data_buffers.clear();
    }

    // create a constant data buffer for each draw command
    for (const auto &command : render_queue_)
    {
        if (command.type() == RenderCommandType::DRAW)
        {
            const auto *command_ptr = std::addressof(command);

            for (auto &frame : frames_)
            {
                frame.constant_data_buffers.emplace(
                    command_ptr, D3D12ConstantBufferPool{});
            }
        }
    }
}

RenderTarget *D3D12Renderer::create_render_target(
    std::uint32_t width,
    std::uint32_t height)
{
    const auto scale = Root::window_manager().current_window()->screen_scale();

    auto colour_texture = std::make_unique<D3D12Texture>(
        DataBuffer{},
        width * scale,
        height * scale,
        TextureUsage::RENDER_TARGET);
    auto depth_texture = std::make_unique<D3D12Texture>(
        DataBuffer{}, width * scale, height * scale, TextureUsage::DEPTH);

    // add these to uploaded so the next render pass doesn't try to upload them
    uploaded_.emplace(colour_texture.get());
    uploaded_.emplace(depth_texture.get());

    render_targets_.emplace_back(std::make_unique<D3D12RenderTarget>(
        std::move(colour_texture), std::move(depth_texture)));

    auto *rt = render_targets_.back().get();

    return rt;
}

void D3D12Renderer::pre_render()
{
    const auto &frame = frames_[frame_index_];

    // if the gpu is still using this frame then wait
    ::WaitForSingleObject(frame.fence_event, INFINITE);

    // reset state to non-waiting
    frame.fence->Signal(0u);
    ::ResetEvent(frame.fence_event);

    // reset command allocator and list for new frame
    frame.command_allocator->Reset();
    command_list_->Reset(frame.command_allocator.Get(), nullptr);

    // reset descriptor allocations for new frame
    D3D12DescriptorManager::gpu_allocator(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
        .reset();

    D3D12DescriptorManager::cpu_allocator(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
        .reset_dynamic();
}

void D3D12Renderer::execute_upload_texture(RenderCommand &command)
{
    const auto *material =
        static_cast<const D3D12Material *>(command.material());

    // encode commands to copy all textures to their target heaps
    for (auto *texture : material->textures())
    {
        const auto *d3d12_tex = static_cast<const D3D12Texture *>(texture);

        // only upload once
        if (uploaded_.count(d3d12_tex) == 0u)
        {
            uploaded_.emplace(d3d12_tex);

            D3D12_TEXTURE_COPY_LOCATION destination = {};
            destination.pResource = d3d12_tex->resource();
            destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            destination.SubresourceIndex = 0u;

            D3D12_TEXTURE_COPY_LOCATION source = {};
            source.pResource = d3d12_tex->upload();
            source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            source.PlacedFootprint = d3d12_tex->footprint();

            command_list_->CopyTextureRegion(
                &destination, 0u, 0u, 0u, &source, NULL);

            const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_tex->resource(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            command_list_->ResourceBarrier(1u, &barrier);
        }
    }
}

void D3D12Renderer::execute_pass_start(RenderCommand &command)
{
    ID3D12DescriptorHeap *heaps[] = {D3D12DescriptorManager::gpu_allocator(
                                         D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
                                         .heap()};
    command_list_->SetDescriptorHeaps(_countof(heaps), heaps);

    const Colour clear_colour{0.4f, 0.6f, 0.9f, 1.0f};

    D3D12_CPU_DESCRIPTOR_HANDLE rt_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE depth_handle;

    auto *target = static_cast<const D3D12RenderTarget *>(
        command.render_pass()->render_target);

    const auto scale = Root::window_manager().current_window()->screen_scale();
    auto width = width_ * scale;
    auto height = height_ * scale;

    const auto &frame = frames_[frame_index_];

    if (target == nullptr)
    {
        rt_handle = frame.render_target.cpu_handle();
        depth_handle = frame.depth_buffer->depth_handle().cpu_handle();

        // if the current frame is the default render target i.e. not one
        // manually created we need to transition it from PRESENT and make the
        // depth buffer writable
        const D3D12_RESOURCE_BARRIER barriers[] = {
            ::CD3DX12_RESOURCE_BARRIER::Transition(
                frame.buffer.Get(),
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET),
            ::CD3DX12_RESOURCE_BARRIER::Transition(
                frame.depth_buffer->resource(),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_DEPTH_WRITE)};

        command_list_->ResourceBarrier(2u, barriers);
    }
    else
    {
        width = target->width();
        height = target->height();

        rt_handle = static_cast<const D3D12RenderTarget *>(target)
                        ->handle()
                        .cpu_handle();
        depth_handle = static_cast<D3D12Texture *>(target->depth_texture())
                           ->depth_handle()
                           .cpu_handle();

        // if we are rendering to a custom render target we just need to make
        // its depth buffer writable

        const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
            static_cast<const D3D12Texture *>(target->depth_texture())
                ->resource(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_DEPTH_WRITE);

        command_list_->ResourceBarrier(1u, &barrier);
    }

    // setup and clear render target
    command_list_->OMSetRenderTargets(1, &rt_handle, FALSE, &depth_handle);
    command_list_->ClearRenderTargetView(
        rt_handle, reinterpret_cast<const FLOAT *>(&clear_colour), 0, nullptr);
    command_list_->ClearDepthStencilView(
        depth_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);

    command_list_->SetGraphicsRootSignature(D3D12Context::root_signature());

    // update viewport incase it's changed for current render target
    viewport_ = CD3DX12_VIEWPORT{
        0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)};
    scissor_rect_ = CD3DX12_RECT{
        0u, 0u, static_cast<LONG>(width), static_cast<LONG>(height)};

    command_list_->RSSetViewports(1u, &viewport_);
    command_list_->RSSetScissorRects(1u, &scissor_rect_);
}

void D3D12Renderer::execute_pass_end(RenderCommand &command)
{
    const auto *target = static_cast<const D3D12RenderTarget *>(
        command.render_pass()->render_target);

    if (target != nullptr)
    {
        // if we are rendering to a custom render target then we need to make
        // the depth buffer accessible to the shader
        const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
            static_cast<const D3D12Texture *>(target->depth_texture())
                ->resource(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        command_list_->ResourceBarrier(1u, &barrier);
    }
}

void D3D12Renderer::execute_draw(RenderCommand &command)
{
    const auto *entity = command.render_entity();
    const auto *material =
        static_cast<const D3D12Material *>(command.material());
    const auto *mesh = static_cast<const D3D12Mesh *>(entity->mesh());
    const auto *camera = command.render_pass()->camera;
    const auto *light = command.light();
    const auto *shadow_map = command.shadow_map();

    command_list_->SetPipelineState(material->pso());

    auto &frame = frames_[frame_index_];

    // create a table descriptor which is a continuous block of all descriptors
    // needed by the shader
    // the number of descriptors is set by the root signature but this is how
    // we organise them:
    //
    //                .-+--------------------+
    //                | |    vertex data     |
    // constant data  | +--------------------+
    //                | |     light data     |
    //                '-+--------------------+-.
    //                  | shadow map texture | |
    //                  +--------------------+ |
    //                  |      texture 1     | |
    //                  +--------------------+ |
    //                  |      texture 2     | | shader resources
    //                  +--------------------+ |
    //                  |      texture 3     | |
    //                  +--------------------+ |
    //                  |      texture 4     | |
    //                  +--------------------+-'
    const auto table_descriptors =
        D3D12DescriptorManager::gpu_allocator(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
            .allocate(D3D12Context::num_descriptors());
    const auto descriptor_size = D3D12DescriptorManager::gpu_allocator(
                                     D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
                                     .descriptor_size();

    auto table_descriptor_start = table_descriptors.cpu_handle();

    // create and write our constant data buffers
    auto &vertex_buffer =
        frame.constant_data_buffers.at(std::addressof(command)).next();
    write_vertex_data_constant_buffer(vertex_buffer, camera, entity, light);

    auto &light_buffer =
        frame.constant_data_buffers.at(std::addressof(command)).next();
    write_directional_light_data_constant_buffer(light_buffer, light);

    // create handles to light and shadow map data, these may be a null handle
    // depending on the material

    const auto light_data_handle = (light->type() == LightType::DIRECTIONAL)
                                       ? light_buffer.descriptor_handle()
                                       : null_buffer_->descriptor_handle();

    auto shadow_map_handle =
        (shadow_map == nullptr)
            ? static_cast<D3D12Texture *>(Root::texture_manager().blank())
                  ->handle()
            : static_cast<D3D12Texture *>(command.shadow_map()->depth_texture())
                  ->handle();

    // build the table descriptor from all our handles
    build_table_descriptor(
        table_descriptor_start,
        descriptor_size,
        vertex_buffer.descriptor_handle(),
        light_buffer.descriptor_handle(),
        shadow_map_handle,
        material->textures());

    // set the table descriptor for the vertex and pixel shader
    command_list_->SetGraphicsRootDescriptorTable(
        0u, table_descriptors.gpu_handle());
    command_list_->SetGraphicsRootDescriptorTable(
        1u, table_descriptors.gpu_handle());

    const auto vertex_view = mesh->vertex_buffer().vertex_view();
    const auto index_view = mesh->index_buffer().index_view();
    const auto num_indices =
        static_cast<UINT>(mesh->index_buffer().element_count());

    switch (entity->primitive_type())
    {
        case PrimitiveType::TRIANGLES:
            command_list_->IASetPrimitiveTopology(
                D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            break;
        case PrimitiveType::LINES:
            command_list_->IASetPrimitiveTopology(
                D3D_PRIMITIVE_TOPOLOGY_LINELIST);
            break;
    }

    command_list_->IASetVertexBuffers(0u, 1u, &vertex_view);
    command_list_->IASetIndexBuffer(&index_view);
    command_list_->DrawIndexedInstanced(num_indices, 1u, 0u, 0u, 0u);
}

void D3D12Renderer::execute_present(RenderCommand &)
{
    const auto &frame = frames_[frame_index_];

    // transition the frame render target to present and depth buffer to shader
    // visible
    const D3D12_RESOURCE_BARRIER barriers[] = {
        ::CD3DX12_RESOURCE_BARRIER::Transition(
            frame.buffer.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT),
        ::CD3DX12_RESOURCE_BARRIER::Transition(
            frame.depth_buffer->resource(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)};

    command_list_->ResourceBarrier(2u, barriers);

    command_list_->Close();

    // execute command list
    ID3D12CommandList *const command_lists[] = {command_list_.Get()};
    command_queue_->ExecuteCommandLists(1u, command_lists);

    // present frame to window
    expect(swap_chain_->Present(0u, 0u) == S_OK, "could not present");

    // enqueue signal so future render passes know when the frame is safe to use
    expect(
        command_queue_->Signal(frame.fence.Get(), 1u) == S_OK,
        "could not signal");
    frame.fence->SetEventOnCompletion(1u, frame.fence_event);

    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();
}

}
