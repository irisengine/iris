////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_constant_buffer.h"

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
#include "graphics/texture_manager.h"

namespace
{

/**
 * Helper function to create a D3D12 buffer on the upload heap.
 *
 * @param frame
 *   The frame number using this buffer.
 *
 * @param capacity
 *   Size of buffer (in bytes).
 *
 * @param resource
 *   A D3D12 handle to be set to the new resource.
 *
 * @returns
 *   Descriptor handle to buffer.
 */
iris::D3D12DescriptorHandle create_resource(
    std::uint32_t frame,
    std::size_t capacity,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resource)
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

    // set a name for the resource
    static int counter = 0;
    std::wstringstream strm{};
    strm << L"cb_" << counter++;
    const auto name = strm.str();

    resource->SetName(name.c_str());

    // allocate descriptor for buffer
    return iris::D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).allocate_dynamic(frame);
}

}

namespace iris
{

D3D12ConstantBuffer::D3D12ConstantBuffer(std::uint32_t frame)
    : capacity_(1u)
    , mapped_buffer_(nullptr)
    , resource_(nullptr)
    , descriptor_handle_(create_resource(frame, capacity_, resource_))
{
    auto *device = iris::D3D12Context::device();

    device->CreateConstantBufferView(nullptr, descriptor_handle_.cpu_handle());

    // note we don't create a view or map the resource, it's a null buffer so
    // there are no valid actions on it
}

D3D12ConstantBuffer::D3D12ConstantBuffer(std::uint32_t frame, std::size_t capacity)
    : capacity_(capacity)
    , mapped_buffer_(nullptr)
    , resource_(nullptr)
    , descriptor_handle_(create_resource(frame, capacity_, resource_))
{
    auto *device = D3D12Context::device();

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_descriptor = {0};
    cbv_descriptor.BufferLocation = resource_->GetGPUVirtualAddress();
    cbv_descriptor.SizeInBytes = static_cast<UINT>(capacity_);

    // create a view onto the buffer
    device->CreateConstantBufferView(&cbv_descriptor, descriptor_handle_.cpu_handle());

    // map the buffer to the cpu so we can write to it
    const auto map_resource = resource_->Map(0u, NULL, reinterpret_cast<void **>(&mapped_buffer_));
    expect(map_resource == S_OK, "failed to map constant buffer");
}

D3D12DescriptorHandle D3D12ConstantBuffer::descriptor_handle() const
{
    return descriptor_handle_;
}

}
