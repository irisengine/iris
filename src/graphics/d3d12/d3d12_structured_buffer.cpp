////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_structured_buffer.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_cpu_descriptor_handle_allocator.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/d3d12/d3d12_gpu_descriptor_handle_allocator.h"

namespace
{

/**
 * Helper function to create a D3D12 buffer on the upload heap.
 *
 * @param capacity
 *   Size of buffer (in bytes).
 *
 * @param resource
 *   A D3D12 handle to be set to the new resource.
 *
 * @param frame
 *   Optional name frame number - if supplied descriptor handle will be allocated for that frame, else it will be
 * statically allocated.
 *
 * @returns
 *   Descriptor handle to buffer.
 */
iris::D3D12DescriptorHandle create_resource(
    std::size_t capacity,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resource,
    std::optional<std::uint32_t> frame)
{
    const auto upload_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto heap_descriptor = CD3DX12_RESOURCE_DESC::Buffer(capacity);

    auto *device = iris::D3D12Context::device();

    // create the buffer
    const auto commit_resource = device->CreateCommittedResource(
        &upload_heap,
        D3D12_HEAP_FLAG_NONE,
        &heap_descriptor,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource));
    iris::expect(commit_resource == S_OK, "could not create constant buffer");

    // name resource
    static int counter = 0;
    std::wstringstream strm{};
    strm << L"sb_" << counter++;
    const auto name = strm.str();
    resource->SetName(name.c_str());

    // allocate descriptor for buffer
    return frame
               ? iris::D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
                     .allocate_dynamic(*frame)
               : iris::D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).allocate_static();
}

}

namespace iris
{

D3D12StructuredBuffer::D3D12StructuredBuffer(std::size_t object_count, std::size_t object_size)
    : capacity_(object_count * object_size)
    , mapped_buffer_(nullptr)
    , resource_(nullptr)
    , descriptor_handle_(create_resource(capacity_, resource_, std::nullopt))
    , static_descriptor_(true)
{
    auto *device = D3D12Context::device();

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_descriptor = {0};
    srv_descriptor.Format = DXGI_FORMAT_UNKNOWN;
    srv_descriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srv_descriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv_descriptor.Buffer.FirstElement = 0;
    srv_descriptor.Buffer.NumElements = static_cast<UINT>(object_count);
    srv_descriptor.Buffer.StructureByteStride = static_cast<UINT>(object_size);
    srv_descriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    // create a view onto the buffer
    device->CreateShaderResourceView(resource_.Get(), &srv_descriptor, descriptor_handle_.cpu_handle());

    // map the buffer to the cpu so we can write to it
    const auto map_resource = resource_->Map(0u, NULL, reinterpret_cast<void **>(&mapped_buffer_));
    expect(map_resource == S_OK, "failed to map constant buffer");
}

D3D12StructuredBuffer::D3D12StructuredBuffer(std::size_t object_count, std::size_t object_size, std::uint32_t frame)
    : capacity_(object_count * object_size)
    , mapped_buffer_(nullptr)
    , resource_(nullptr)
    , descriptor_handle_(create_resource(capacity_, resource_, frame))
    , static_descriptor_(false)
{
    auto *device = D3D12Context::device();

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_descriptor = {0};
    srv_descriptor.Format = DXGI_FORMAT_UNKNOWN;
    srv_descriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srv_descriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv_descriptor.Buffer.FirstElement = 0;
    srv_descriptor.Buffer.NumElements = static_cast<UINT>(object_count);
    srv_descriptor.Buffer.StructureByteStride = static_cast<UINT>(object_size);
    srv_descriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    // create a view onto the buffer
    device->CreateShaderResourceView(resource_.Get(), &srv_descriptor, descriptor_handle_.cpu_handle());

    // map the buffer to the cpu so we can write to it
    const auto map_resource = resource_->Map(0u, NULL, reinterpret_cast<void **>(&mapped_buffer_));
    expect(map_resource == S_OK, "failed to map constant buffer");
}

D3D12StructuredBuffer::~D3D12StructuredBuffer()
{
    if (static_descriptor_)
    {
        iris::D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
            .release_static(descriptor_handle_);
    }
}

D3D12DescriptorHandle D3D12StructuredBuffer::descriptor_handle() const
{
    return descriptor_handle_;
}

}
