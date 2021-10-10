////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_buffer.h"

#include <any>
#include <cstring>

#include <wrl.h>

#include "directx/d3d12.h"

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/vertex_data.h"

namespace
{

/**
 * Helper function to create a new GPU D3D12 buffer.
 *
 * @param size
 *   Size of buffer (in bytes).
 *
 * @param mapped_memory
 *   Out pointer (to a pointer) where the buffer will be mapped to the CPU.
 */
Microsoft::WRL::ComPtr<ID3D12Resource> create_resource(
    std::size_t size,
    std::byte **mapped_memory)
{
    auto *device = iris::D3D12Context::device();

    // create the buffer on the upload heap
    CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_UPLOAD);
    const auto buffer_descriptor =
        CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT>(size));
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

    const auto commit_resource = device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &buffer_descriptor,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource));
    iris::expect(
        commit_resource == S_OK, "could not create committed resource");

    CD3DX12_RANGE read_range(0, 0);

    // map the gpu buffer to the cpu, so it can be written to
    iris::expect(
        resource->Map(
            0u, &read_range, reinterpret_cast<void **>(mapped_memory)) == S_OK,
        "could not map buffer");

    return resource;
}

}

namespace iris
{

D3D12Buffer::D3D12Buffer(const std::vector<VertexData> &vertex_data)
    : resource_()
    , vertex_buffer_view_()
    , index_buffer_view_()
    , element_count_(vertex_data.size())
    , capacity_(element_count_)
    , mapped_memory_(nullptr)
{
    resource_ =
        create_resource(capacity_ * sizeof(VertexData), &mapped_memory_);

    write(vertex_data);

    vertex_buffer_view_.BufferLocation = resource_->GetGPUVirtualAddress();
    vertex_buffer_view_.SizeInBytes =
        static_cast<UINT>(element_count_ * sizeof(VertexData));
    vertex_buffer_view_.StrideInBytes = sizeof(VertexData);
}

D3D12Buffer::D3D12Buffer(const std::vector<std::uint32_t> &index_data)
    : resource_()
    , vertex_buffer_view_()
    , index_buffer_view_()
    , element_count_(index_data.size())
    , capacity_(element_count_)
    , mapped_memory_(nullptr)
{
    resource_ =
        create_resource(capacity_ * sizeof(std::uint32_t), &mapped_memory_);

    write(index_data);

    index_buffer_view_.BufferLocation = resource_->GetGPUVirtualAddress();
    index_buffer_view_.SizeInBytes =
        static_cast<UINT>(element_count_ * sizeof(std::uint32_t));
    index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
}

std::size_t D3D12Buffer::element_count() const
{
    return element_count_;
}

D3D12_VERTEX_BUFFER_VIEW D3D12Buffer::vertex_view() const
{
    return vertex_buffer_view_;
}

D3D12_INDEX_BUFFER_VIEW D3D12Buffer::index_view() const
{
    return index_buffer_view_;
}

void D3D12Buffer::write(const std::vector<VertexData> &vertex_data)
{
    element_count_ = vertex_data.size();

    // if buffer is too small for new data then reallocate
    if (element_count_ > capacity_)
    {
        capacity_ = element_count_;
        resource_ =
            create_resource(capacity_ * sizeof(VertexData), &mapped_memory_);

        vertex_buffer_view_.BufferLocation = resource_->GetGPUVirtualAddress();
        vertex_buffer_view_.SizeInBytes =
            static_cast<UINT>(capacity_ * sizeof(VertexData));
        vertex_buffer_view_.StrideInBytes = sizeof(VertexData);
    }

    // copy new data
    std::memcpy(
        mapped_memory_,
        vertex_data.data(),
        element_count_ * sizeof(VertexData));
}

void D3D12Buffer::write(const std::vector<std::uint32_t> &index_data)
{
    element_count_ = index_data.size();

    // if buffer is too small for new data then reallocate
    if (element_count_ > capacity_)
    {
        capacity_ = element_count_;
        resource_ =
            create_resource(capacity_ * sizeof(std::uint32_t), &mapped_memory_);

        index_buffer_view_.BufferLocation = resource_->GetGPUVirtualAddress();
        index_buffer_view_.SizeInBytes =
            static_cast<UINT>(capacity_ * sizeof(std::uint32_t));
        index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
    }

    // copy new data
    std::memcpy(
        mapped_memory_,
        index_data.data(),
        element_count_ * sizeof(std::uint32_t));
}

}
