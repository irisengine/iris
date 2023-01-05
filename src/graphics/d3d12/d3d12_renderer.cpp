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
#include <deque>
#include <iostream>
#include <map>
#include <ranges>
#include <unordered_map>
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
#include "graphics/d3d12/d3d12_cube_map.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/d3d12/d3d12_material.h"
#include "graphics/d3d12/d3d12_mesh.h"
#include "graphics/d3d12/d3d12_render_target.h"
#include "graphics/d3d12/d3d12_sampler.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/mesh_manager.h"
#include "graphics/render_command_type.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/single_entity.h"
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
// d3d12 NDC
static const iris::Matrix4 directx_translate{
    {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f}};

/**
 * Helper function to write entity data to buffers.
 *
 * @param bone_buffer
 *   D3D12ConstantBuffer object to write bone data to.
 *
 * @param model_buffer
 *   D3D12StructuredBuffer object to write model data to.
 *
 * @param entity
 *   Entity being rendered.
 */
void write_entity_data_constant_buffer(
    iris::D3D12ConstantBuffer &bone_buffer,
    iris::D3D12StructuredBuffer &model_buffer,
    const iris::RenderEntity *entity)
{
    static std::vector<iris::Matrix4> default_bones(100u);

    iris::ConstantBufferWriter writer(bone_buffer);

    if (entity->type() == iris::RenderEntityType::SINGLE)
    {
        const auto *single_entity = static_cast<const iris::SingleEntity *>(entity);

        if (single_entity->skeleton() != nullptr)
        {
            const auto &bones = single_entity->skeleton()->transforms();
            writer.write(bones);
        }
        else
        {
            writer.write(default_bones);
        }

        iris::ConstantBufferWriter writer2(model_buffer);
        writer2.write(single_entity->transform());
        writer2.write(single_entity->normal_transform());
    }
    else
    {
        writer.write(default_bones);
    }
}

/**
 * Helper function to write light specific data to a constant buffer.
 *
 * @param constant_buffer
 *   D3D12ConstantBuffer object to write to.
 *
 * @param light
 *   Light to get data from.
 */
void write_light_data_constant_buffer(iris::D3D12ConstantBuffer &constant_buffer, const iris::Light *light)
{
    iris::ConstantBufferWriter writer(constant_buffer);

    if (light->type() == iris::LightType::DIRECTIONAL)
    {
        const auto *d3d12_light = static_cast<const iris::DirectionalLight *>(light);

        writer.write(directx_translate * d3d12_light->shadow_camera().projection());
        writer.write(d3d12_light->shadow_camera().view());
    }
    else
    {
        writer.advance(sizeof(iris::Matrix4) * 2u);
    }

    writer.write(light->colour_data());
    writer.write(light->world_space_data());
    writer.write(light->attenuation_data());
    writer.write(0.0f);
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
 *
 * @param heap_type
 *   Type of heap being copied to.
 */
void copy_descriptor(
    D3D12_CPU_DESCRIPTOR_HANDLE &dest,
    const iris::D3D12DescriptorHandle &source,
    std::size_t descriptor_size,
    D3D12_DESCRIPTOR_HEAP_TYPE heap_type)
{
    auto *device = iris::D3D12Context::device();

    device->CopyDescriptorsSimple(1u, dest, source.cpu_handle(), heap_type);

    dest.ptr += descriptor_size;
}

/**
 * Helper function to encode commands for uploading textures.
 *
 * @param uploaded_textures
 *   Set of all textures that have been uploaded (will be updated).
 *
 * @param uploaded_cube_maps
 *   Set of all cube maps that have been uploaded (will be updated).
 *
 * @param command_list
 *   D3D12 command list to encode command to.
 */
void upload_textures(
    std::set<const iris::D3D12Texture *> &uploaded_textures,
    std::set<const iris::D3D12CubeMap *> &uploaded_cube_maps,
    ID3D12GraphicsCommandList *command_list)
{
    // encode commands to copy all textures to their target heaps
    for (auto *texture : iris::Root::texture_manager().textures() | std::views::filter([](const auto *element) {
                             return !(
                                 (element->usage() == iris::TextureUsage::RENDER_TARGET) ||
                                 (element->usage() == iris::TextureUsage::DEPTH));
                         }))
    {
        const auto *d3d12_tex = static_cast<const iris::D3D12Texture *>(texture);

        // only upload once
        if (!uploaded_textures.contains(d3d12_tex))
        {
            uploaded_textures.emplace(d3d12_tex);

            const auto footprints = d3d12_tex->footprints();

            for (auto i = 0u; i < footprints.size(); ++i)
            {
                D3D12_TEXTURE_COPY_LOCATION destination = {};
                destination.pResource = d3d12_tex->resource();
                destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                destination.SubresourceIndex = i;

                D3D12_TEXTURE_COPY_LOCATION source = {};
                source.pResource = d3d12_tex->upload();
                source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                source.PlacedFootprint = footprints[i];

                command_list->CopyTextureRegion(&destination, 0u, 0u, 0u, &source, NULL);
            }

            const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_tex->resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            command_list->ResourceBarrier(1u, &barrier);
        }
    }

    // encode commands to copy all cube maps to their target heaps
    for (auto *cube_map : iris::Root::texture_manager().cube_maps())
    {
        const auto *d3d12_cube_map = static_cast<const iris::D3D12CubeMap *>(cube_map);

        // encode commands to copy all cube map textures to their target heaps
        if (!uploaded_cube_maps.contains(d3d12_cube_map))
        {
            uploaded_cube_maps.emplace(d3d12_cube_map);

            // encode a copy command for each face
            UINT index = 0u;
            for (const auto &footprint : d3d12_cube_map->footprints())
            {
                D3D12_TEXTURE_COPY_LOCATION destination = {};
                destination.pResource = d3d12_cube_map->resource();
                destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                destination.SubresourceIndex = index++;

                D3D12_TEXTURE_COPY_LOCATION source = {};
                source.pResource = d3d12_cube_map->upload();
                source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                source.PlacedFootprint = footprint;

                command_list->CopyTextureRegion(&destination, 0u, 0u, 0u, &source, NULL);
            }

            const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_cube_map->resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            command_list->ResourceBarrier(1u, &barrier);
        }
    }
}

/**
 * Helper function to build the texture table - a global GPU buffer of all textures (used for bindless rendering)
 *
 * @returns
 *   D3D12DescriptorHandle to a first element in table with all loaded textures.
 */
iris::D3D12DescriptorHandle create_texture_table()
{
    const auto textures = iris::Root::texture_manager().textures();
    const auto max_index = textures.back()->index();
    const auto *blank_texture = static_cast<const iris::D3D12Texture *>(iris::Root::texture_manager().blank_texture());
    auto iter = std::cbegin(textures);

    // create table buffer
    auto texture_table = iris::D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
                             .allocate_static(static_cast<std::uint32_t>(max_index + 1u));
    const auto descriptor_size =
        iris::D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).descriptor_size();
    auto texture_table_descriptor_start = texture_table.cpu_handle();

    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a texture exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *d3d12_texture = static_cast<const iris::D3D12Texture *>(*iter);
            copy_descriptor(
                texture_table_descriptor_start,
                d3d12_texture->handle(),
                descriptor_size,
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            ++iter;
        }
        else
        {
            // no texture at current index, so write default texture
            copy_descriptor(
                texture_table_descriptor_start,
                blank_texture->handle(),
                descriptor_size,
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
    }

    return texture_table;
}

/**
 * Helper function to build the cube map table - a global GPU buffer of all cube maps (used for bindless rendering)
 *
 * @returns
 *   D3D12DescriptorHandle to a first element in table with all loaded cube maps.
 */
iris::D3D12DescriptorHandle create_cube_map_table()
{
    const auto cube_maps = iris::Root::texture_manager().cube_maps();
    const auto max_index = cube_maps.back()->index();
    const auto *blank_cube_map =
        static_cast<const iris::D3D12CubeMap *>(iris::Root::texture_manager().blank_cube_map());
    auto iter = std::cbegin(cube_maps);

    auto cube_map_table = iris::D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
                              .allocate_static(static_cast<std::uint32_t>(max_index + 1u));
    const auto descriptor_size =
        iris::D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).descriptor_size();
    auto cube_map_table_descriptor_start = cube_map_table.cpu_handle();

    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a cube map exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *d3d12_cube_map = static_cast<const iris::D3D12CubeMap *>(*iter);
            copy_descriptor(
                cube_map_table_descriptor_start,
                d3d12_cube_map->handle(),
                descriptor_size,
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            ++iter;
        }
        else
        {
            // no cube map at current index, so write default cube map
            copy_descriptor(
                cube_map_table_descriptor_start,
                blank_cube_map->handle(),
                descriptor_size,
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
    }

    return cube_map_table;
}

/**
 * Helper function to build the sampler table - a global GPU buffer of all samplers (used for bindless rendering)
 *
 * @returns
 *   D3D12DescriptorHandle to a first element in table with all loaded samplers.
 */
iris::D3D12DescriptorHandle create_sampler_table()
{
    const auto samplers = iris::Root::texture_manager().samplers();
    const auto max_index = samplers.back()->index();
    const auto *default_sampler =
        static_cast<const iris::D3D12Sampler *>(iris::Root::texture_manager().default_texture_sampler());
    auto iter = std::cbegin(samplers);

    // create table buffer
    auto sampler_table = iris::D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
                             .allocate_static(static_cast<std::uint32_t>(max_index + 1u));
    const auto descriptor_size =
        iris::D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).descriptor_size();
    auto sampler_table_descriptor_start = sampler_table.cpu_handle();

    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a sampler exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *d3d12_sampler = static_cast<const iris::D3D12Sampler *>(*iter);
            copy_descriptor(
                sampler_table_descriptor_start,
                d3d12_sampler->handle(),
                descriptor_size,
                D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            ++iter;
        }
        else
        {
            // no sampler at current index, so write default sampler
            copy_descriptor(
                sampler_table_descriptor_start,
                default_sampler->handle(),
                descriptor_size,
                D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        }
    }

    return sampler_table;
}

}

namespace iris
{

D3D12Renderer::D3D12Renderer(HWND window, std::uint32_t width, std::uint32_t height, std::uint32_t initial_screen_scale)
    : width_(width)
    , height_(height)
    , frames_()
    , frame_index_(0u)
    , fence_(nullptr)
    , fence_event_()
    , command_queue_(nullptr)
    , command_list_(nullptr)
    , swap_chain_(nullptr)
    , viewport_()
    , scissor_rect_()
    , uploaded_textures_()
    , uploaded_cube_maps_()
    , instance_data_buffers_()
    , texture_table_()
    , cube_map_table_()
    , sampler_table_()
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
    ensure(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue_)) == S_OK, "could not create command queue");

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
            command_queue_.Get(), window, &swap_chain_descriptor, nullptr, nullptr, &swap_chain_tmp) == S_OK,
        "could not create swap chain");

    // cast to type we want to use
    ensure(swap_chain_tmp.As(&swap_chain_) == S_OK, "could not cast swap chain");

    // get initial frame index
    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    // build our frame buffers
    for (auto i = 0u; i < num_frames; ++i)
    {
        // get a back buffer
        Microsoft::WRL::ComPtr<ID3D12Resource> frame = nullptr;
        ensure(swap_chain_->GetBuffer(i, IID_PPV_ARGS(&frame)) == S_OK, "could not get back buffer");

        static int counter = 0;
        std::wstringstream strm{};
        strm << L"frame_" << counter++;
        const auto name = strm.str();
        frame->SetName(name.c_str());

        // create a static descriptor handle for the render target
        auto rtv_handle = D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).allocate_static();

        device->CreateRenderTargetView(frame.Get(), nullptr, rtv_handle.cpu_handle());

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;

        // create command allocator, we use one per frame but have a single
        // command queue
        ensure(
            device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)) == S_OK,
            "could not create command allocator");

        const auto *rt_sampler = Root::texture_manager().create(SamplerDescriptor{
            .s_address_mode = SamplerAddressMode::CLAMP_TO_BORDER,
            .t_address_mode = SamplerAddressMode::CLAMP_TO_BORDER,
            .border_colour = Colour{1.0f, 1.0f, 1.0f, 1.0f},
            .uses_mips = false});

        frames_.emplace_back(
            i,
            frame,
            rtv_handle,
            std::make_unique<D3D12Texture>(
                DataBuffer{},
                width_ * initial_screen_scale,
                height_ * initial_screen_scale,
                rt_sampler,
                TextureUsage::DEPTH,
                Root::texture_manager().next_texture_index()),
            command_allocator);
    }

    // create a fence, used to signal when gpu has completed a frame
    ensure(
        device->CreateFence(frames_[frame_index_].fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)) == S_OK,
        "could not create fence");
    fence_event_ = {::CreateEventA(NULL, FALSE, FALSE, NULL), ::CloseHandle};
    ++frames_[frame_index_].fence_value;

    command_queue_->Signal(fence_.Get(), frames_[frame_index_].fence_value);
    fence_->SetEventOnCompletion(frames_[frame_index_].fence_value, fence_event_);
    ::WaitForSingleObject(fence_event_, INFINITE);
    ++frames_[frame_index_].fence_value;

    ensure(
        device->CreateCommandList(
            0u,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            frames_[frame_index_].command_allocator.Get(),
            nullptr,
            IID_PPV_ARGS(&command_list_)) == S_OK,
        "could not create command list");

    // close the list so we can start recording to it
    command_list_->Close();
}

D3D12Renderer::~D3D12Renderer()
{
    // block and wait for current frame to finish - this prevents us unloading resources whilst the gpu is still using
    // them
    command_queue_->Signal(fence_.Get(), frames_[frame_index_].fence_value);
    fence_->SetEventOnCompletion(frames_[frame_index_].fence_value, fence_event_);
    ::WaitForSingleObject(fence_event_, INFINITE);
    ++frames_[frame_index_].fence_value;
}

void D3D12Renderer::do_set_render_pipeline(std::function<void()> build_queue)
{
    command_queue_->Signal(fence_.Get(), frames_[frame_index_].fence_value);
    fence_->SetEventOnCompletion(frames_[frame_index_].fence_value, fence_event_);
    ::WaitForSingleObject(fence_event_, INFINITE);
    ++frames_[frame_index_].fence_value;

    build_queue();

    instance_data_buffers_.clear();

    for (auto &frame : frames_)
    {
        frame.model_data_buffers.clear();
    }

    // create structured buffers for any instanced entities in the scene
    for (const auto &command : render_queue_)
    {
        if (command.type() == RenderCommandType::DRAW)
        {
            const auto *render_entity = command.render_entity();

            if (render_entity->type() == RenderEntityType::INSTANCED)
            {
                const auto *instanced_entity = static_cast<const InstancedEntity *>(render_entity);
                instance_data_buffers_[render_entity] =
                    std::make_unique<D3D12StructuredBuffer>(instanced_entity->data().size(), sizeof(Matrix4) * 2u);

                ConstantBufferWriter writer{*instance_data_buffers_[render_entity]};

                writer.write(instanced_entity->data());
            }
        }
    }

    texture_table_ = create_texture_table();
    cube_map_table_ = create_cube_map_table();
    sampler_table_ = create_sampler_table();
}

void D3D12Renderer::pre_render()
{
    auto &frame = frames_[frame_index_];

    // reset command allocator and list for new frame
    frame.command_allocator->Reset();
    command_list_->Reset(frame.command_allocator.Get(), nullptr);

    // reset descriptor allocations for new frame
    D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).reset_dynamic(frame.frame_id);
    D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).reset_dynamic(frame.frame_id);

    // upload any new textures
    upload_textures(uploaded_textures_, uploaded_cube_maps_, command_list_.Get());

    // clear frame specific buffers
    frame.bone_data_buffers.clear();
    frame.light_data_buffers.clear();
    frame.camera_data_buffers.clear();
    frame.property_buffers.clear();
}

void D3D12Renderer::execute_pass_start(RenderCommand &command)
{
    ID3D12DescriptorHeap *heaps[] = {
        D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).heap(),
        D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).heap()};
    command_list_->SetDescriptorHeaps(_countof(heaps), heaps);

    // setup the render targets, which are:
    // - colour
    // - screen space normals
    // - screen space positions
    //
    // note that they may not all be used
    std::uint32_t rt_count = 1u;
    D3D12_CPU_DESCRIPTOR_HANDLE rt_handles[3];
    D3D12_CPU_DESCRIPTOR_HANDLE depth_handle;

    auto *colour_target = static_cast<const D3D12RenderTarget *>(command.render_pass()->colour_target);
    auto *normal_target = static_cast<const D3D12RenderTarget *>(command.render_pass()->normal_target);
    auto *position_target = static_cast<const D3D12RenderTarget *>(command.render_pass()->position_target);

    const auto scale = Root::window_manager().current_window()->screen_scale();
    auto width = width_ * scale;
    auto height = height_ * scale;

    auto &frame = frames_[frame_index_];

    if (colour_target == nullptr)
    {
        rt_handles[0] = frame.render_target.cpu_handle();
        depth_handle = frame.depth_buffer->depth_handle().cpu_handle();

        // if the current frame is the default render target i.e. not one
        // manually created we need to transition it from PRESENT and make the
        // depth buffer writable
        const D3D12_RESOURCE_BARRIER barriers[] = {
            ::CD3DX12_RESOURCE_BARRIER::Transition(
                frame.buffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            ::CD3DX12_RESOURCE_BARRIER::Transition(
                frame.depth_buffer->resource(),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_DEPTH_WRITE)};

        command_list_->ResourceBarrier(2u, barriers);
    }
    else
    {
        width = colour_target->width();
        height = colour_target->height();

        rt_handles[0] = colour_target->handle().cpu_handle();
        depth_handle = static_cast<const D3D12Texture *>(colour_target->depth_texture())->depth_handle().cpu_handle();

        // if we are rendering to a custom render target we just need to make
        // its depth buffer writable

        const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
            static_cast<const D3D12Texture *>(colour_target->depth_texture())->resource(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_DEPTH_WRITE);

        command_list_->ResourceBarrier(1u, &barrier);
    }

    if (normal_target != nullptr)
    {
        rt_handles[1] = normal_target->handle().cpu_handle();
        rt_count = 2u;
    }

    if (position_target != nullptr)
    {
        rt_handles[2] = position_target->handle().cpu_handle();
        rt_count = 3u;
    }

    command_list_->OMSetRenderTargets(rt_count, rt_handles, FALSE, &depth_handle);

    // clear any supplied targets
    if (command.render_pass()->clear_colour)
    {
        static const Colour clear_colour{0.4f, 0.6f, 0.9f, 1.0f};
        command_list_->ClearRenderTargetView(rt_handles[0], reinterpret_cast<const FLOAT *>(&clear_colour), 0, nullptr);

        // for the extra targets we clear to black
        // this technically issues a d3d12 warning (which we supress) as clearing to a colour other that the one set
        // when the resource was created is less efficient, however:
        //  - for how often we will be doing it we will take the hit on performance
        //  - it simplifies the code

        if (normal_target != nullptr)
        {
            static const Colour normal_clear_colour{0.0f, 0.0f, 0.0f, 0.0f};
            command_list_->ClearRenderTargetView(
                rt_handles[1], reinterpret_cast<const FLOAT *>(&normal_clear_colour), 0, nullptr);
        }

        if (position_target != nullptr)
        {
            static const Colour position_clear_colour{0.0f, 0.0f, 0.0f, 0.0f};
            command_list_->ClearRenderTargetView(
                rt_handles[2], reinterpret_cast<const FLOAT *>(&position_clear_colour), 0, nullptr);
        }
    }
    if (command.render_pass()->clear_depth)
    {
        command_list_->ClearDepthStencilView(depth_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);
    }

    command_list_->SetGraphicsRootSignature(D3D12Context::root_signature().handle());

    // update viewport incase it's changed for current render target
    viewport_ = CD3DX12_VIEWPORT{0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)};
    scissor_rect_ = CD3DX12_RECT{0u, 0u, static_cast<LONG>(width), static_cast<LONG>(height)};

    command_list_->RSSetViewports(1u, &viewport_);
    command_list_->RSSetScissorRects(1u, &scissor_rect_);
}

void D3D12Renderer::execute_pass_end(RenderCommand &command)
{
    const auto *target = static_cast<const D3D12RenderTarget *>(command.render_pass()->colour_target);

    if (target != nullptr)
    {
        // if we are rendering to a custom render target then we need to make
        // the depth buffer accessible to the shader
        const auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(
            static_cast<const D3D12Texture *>(target->depth_texture())->resource(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        command_list_->ResourceBarrier(1u, &barrier);
    }
}

void D3D12Renderer::execute_draw(RenderCommand &command)
{
    const auto *entity = command.render_entity();
    const auto *material = static_cast<const D3D12Material *>(command.material());
    const auto *mesh = static_cast<const D3D12Mesh *>(entity->mesh());
    const auto *light = command.light();
    const auto *camera = command.render_pass()->camera;

    command_list_->SetPipelineState(material->pso());

    auto &frame = frames_[frame_index_];

    // any frame specific buffers that we are seeing for the first time need to be created

    if (!frame.bone_data_buffers.contains(entity))
    {
        frame.bone_data_buffers[entity] = std::make_unique<D3D12ConstantBuffer>(frame_index_, 7168u);
        frame.model_data_buffers[entity] =
            std::make_unique<D3D12StructuredBuffer>(1u, sizeof(Matrix4) * 2u, frame_index_);

        write_entity_data_constant_buffer(*frame.bone_data_buffers[entity], *frame.model_data_buffers[entity], entity);
    }

    if (!frame.light_data_buffers.contains(light))
    {
        frame.light_data_buffers[light] = std::make_unique<D3D12ConstantBuffer>(frame_index_, 256u);
        write_light_data_constant_buffer(*frame.light_data_buffers[light], light);
    }

    if (!frame.camera_data_buffers.contains(camera))
    {
        frame.camera_data_buffers[camera] = std::make_unique<D3D12ConstantBuffer>(frame_index_, 512u);

        // calculate view matrix for normals
        auto normal_view = Matrix4::transpose(Matrix4::invert(camera->view()));
        normal_view[3] = 0.0f;
        normal_view[7] = 0.0f;
        normal_view[11] = 0.0f;

        ConstantBufferWriter writer{*frame.camera_data_buffers[camera]};
        writer.write(directx_translate * camera->projection());
        writer.write(camera->view());
        writer.write(normal_view);
        writer.write(camera->position());
    }

    if (!frame.property_buffers.contains(material))
    {
        const auto property_buffer = material->property_buffer();
        frame.property_buffers[material] =
            std::make_unique<D3D12ConstantBuffer>(frame_index_, property_buffer.size_bytes());
        frame.property_buffers[material]->write(property_buffer.data(), property_buffer.size_bytes(), 0u);
    }

    auto *bone_buffer = frame.bone_data_buffers[entity].get();
    auto *light_buffer = frame.light_data_buffers[light].get();
    auto *model_buffer = (entity->type() != RenderEntityType::INSTANCED) ? frame.model_data_buffers[entity].get()
                                                                         : instance_data_buffers_[entity].get();
    auto *property_buffer = frame.property_buffers[material].get();
    auto *camera_buffer = frame.camera_data_buffers[camera].get();
    const auto shadow_map_index =
        (command.shadow_map() == nullptr) ? 0u : command.shadow_map()->depth_texture()->index();
    const auto shadow_map_sampler_index =
        (command.shadow_map() == nullptr) ? 0u : command.shadow_map()->depth_texture()->sampler()->index();

    const auto time_value = static_cast<float>(time().count()) / 1000.0f;

    // encode all out root signature arguments
    D3D12Context::root_signature().encode_arguments(
        command_list_.Get(),
        bone_buffer,
        light_buffer,
        camera_buffer,
        shadow_map_index,
        shadow_map_sampler_index,
        std::bit_cast<std::uint32_t>(time_value),
        property_buffer,
        model_buffer,
        texture_table_,
        cube_map_table_,
        sampler_table_);

    const auto vertex_view = mesh->vertex_buffer().vertex_view();
    const auto index_view = mesh->index_buffer().index_view();
    const auto num_indices = static_cast<UINT>(mesh->index_buffer().element_count());

    switch (entity->primitive_type())
    {
        case PrimitiveType::TRIANGLES:
            command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            break;
        case PrimitiveType::LINES: command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); break;
    }

    const auto instance_count = (entity->type() == RenderEntityType::INSTANCED)
                                    ? static_cast<const InstancedEntity *>(entity)->instance_count()
                                    : 1u;

    command_list_->IASetVertexBuffers(0u, 1u, &vertex_view);
    command_list_->IASetIndexBuffer(&index_view);
    command_list_->DrawIndexedInstanced(num_indices, static_cast<UINT>(instance_count), 0u, 0u, 0u);
}

void D3D12Renderer::execute_present(RenderCommand &)
{
    auto &frame = frames_[frame_index_];

    // transition the frame render target to present and depth buffer to shader
    // visible
    const D3D12_RESOURCE_BARRIER barriers[] = {
        ::CD3DX12_RESOURCE_BARRIER::Transition(
            frame.buffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
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
    expect(swap_chain_->Present(0u, DXGI_PRESENT_ALLOW_TEARING) == S_OK, "could not present");

    const auto fence_value = frame.fence_value;
    expect(command_queue_->Signal(fence_.Get(), fence_value) == S_OK, "could not signal");

    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    if (fence_->GetCompletedValue() < frames_[frame_index_].fence_value)
    {
        fence_->SetEventOnCompletion(frames_[frame_index_].fence_value, fence_event_);
        ::WaitForSingleObject(fence_event_, INFINITE);
    }

    frames_[frame_index_].fence_value = fence_value + 1u;
}

}
