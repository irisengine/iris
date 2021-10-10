#include "graphics/d3d12/d3d12_gpu_descriptor_handle_allocator.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"

namespace iris
{

D3D12GPUDescriptorHandleAllocator::D3D12GPUDescriptorHandleAllocator(
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    std::uint32_t num_descriptors)
    : descriptor_heap_(nullptr)
    , cpu_start_()
    , gpu_start_()
    , descriptor_size_(0u)
    , index_(0u)
    , capacity_(num_descriptors)
{
    auto *device = D3D12Context::device();

    // setup heap description
    D3D12_DESCRIPTOR_HEAP_DESC heap_description;
    heap_description.NumDescriptors = num_descriptors;
    heap_description.Type = type;
    heap_description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heap_description.NodeMask = 0;

    // create heap
    expect(
        device->CreateDescriptorHeap(
            &heap_description, IID_PPV_ARGS(&descriptor_heap_)) == S_OK,
        "could not create descriptor heap");

    cpu_start_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
    gpu_start_ = descriptor_heap_->GetGPUDescriptorHandleForHeapStart();

    descriptor_size_ = device->GetDescriptorHandleIncrementSize(type);
}

D3D12DescriptorHandle D3D12GPUDescriptorHandleAllocator::allocate(
    std::uint32_t count)
{
    expect(index_ + count <= capacity_, "heap too small");

    auto cpu_handle = cpu_start_;
    cpu_handle.ptr += descriptor_size_ * index_;

    auto gpu_handle = gpu_start_;
    gpu_handle.ptr += descriptor_size_ * index_;

    index_ += count;

    return {cpu_handle, gpu_handle};
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12GPUDescriptorHandleAllocator::cpu_start() const
{
    return cpu_start_;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12GPUDescriptorHandleAllocator::gpu_start() const
{
    return gpu_start_;
}

std::uint32_t D3D12GPUDescriptorHandleAllocator::descriptor_size() const
{
    return descriptor_size_;
}

ID3D12DescriptorHeap *D3D12GPUDescriptorHandleAllocator::heap() const
{
    return descriptor_heap_.Get();
}

void D3D12GPUDescriptorHandleAllocator::reset()
{
    index_ = 0u;
}

}
