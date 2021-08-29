#include "graphics/d3d12/d3d12_cpu_descriptor_handle_allocator.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/exception.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"

namespace iris
{

D3D12CPUDescriptorHandleAllocator::D3D12CPUDescriptorHandleAllocator(
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    std::uint32_t num_descriptors,
    std::uint32_t static_size,
    bool shader_visible)
    : descriptor_heap_(nullptr)
    , heap_start_()
    , descriptor_size_(0u)
    , static_index_(0u)
    , dynamic_index_(static_size)
    , static_capacity_(static_size)
    , dynamic_capacity_(num_descriptors - static_size)
{
    auto *device = D3D12Context::device();

    // setup heap description
    D3D12_DESCRIPTOR_HEAP_DESC heap_description;
    heap_description.NumDescriptors = num_descriptors;
    heap_description.Type = type;
    heap_description.Flags = shader_visible
                                 ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
                                 : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heap_description.NodeMask = 0;

    // create heap
    if (device->CreateDescriptorHeap(
            &heap_description, IID_PPV_ARGS(&descriptor_heap_)) != S_OK)
    {
        throw Exception("could not create descriptor heap");
    }

    descriptor_size_ = device->GetDescriptorHandleIncrementSize(type);

    heap_start_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
}

D3D12DescriptorHandle D3D12CPUDescriptorHandleAllocator::allocate_static()
{
    if (static_index_ == static_capacity_)
    {
        throw Exception("heap too small");
    }

    // get next free descriptor form static pool
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap_start_;
    cpu_handle.ptr += descriptor_size_ * static_index_;

    ++static_index_;

    return {cpu_handle};
}

D3D12DescriptorHandle D3D12CPUDescriptorHandleAllocator::allocate_dynamic()
{
    if (dynamic_index_ == dynamic_capacity_)
    {
        throw Exception("heap too small");
    }

    // get next free descriptor form dynamic pool
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap_start_;
    cpu_handle.ptr += descriptor_size_ * (static_capacity_ + dynamic_index_);

    ++dynamic_index_;

    return {cpu_handle};
}

std::uint32_t D3D12CPUDescriptorHandleAllocator::descriptor_size() const
{
    return descriptor_size_;
}

void D3D12CPUDescriptorHandleAllocator::reset_dynamic()
{
    dynamic_index_ = 0u;
}

}
