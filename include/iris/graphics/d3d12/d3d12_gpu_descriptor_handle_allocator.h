////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
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

/**
 * This class provides the mechanisms for allocating gpu descriptor heaps and descriptor handles. An instance of this
 * class will pre-allocate a fixed sized pool of descriptors for a given d3d12 heap type, it then provides methods for
 * allocating handles from this pool.
 *
 * It maintains a separate pool per frame in the swap chain.
 *
 * */
template <std::uint32_t N>
class D3D12GPUDescriptorHandleAllocator
{
  public:
    /**
     * Construct a new D3D12GPUDescriptorHandleAllocator.
     *
     * @param type
     *   D3D12 heap type to create.
     *
     * @param num_descriptors
     *   Number of descriptors to create in heap (will be shared among each frame).
     *
     */
    D3D12GPUDescriptorHandleAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, std::uint32_t num_descriptors)
        : descriptor_heap_(nullptr)
        , cpu_start_()
        , gpu_start_()
        , descriptor_size_(0u)
        , indices_()
        , capacity_(num_descriptors / N)
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
            device->CreateDescriptorHeap(&heap_description, IID_PPV_ARGS(&descriptor_heap_)) == S_OK,
            "could not create descriptor heap");

        cpu_start_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
        gpu_start_ = descriptor_heap_->GetGPUDescriptorHandleForHeapStart();

        descriptor_size_ = device->GetDescriptorHandleIncrementSize(type);

        // set the indices for each frames pool
        for (auto i = 0u; i < N; ++i)
        {
            indices_[i] = i * capacity_;
        }
    }

    /**
     * Allocate a contiguous  range of descriptor handles.
     *
     * @param frame
     *   Frame number to allocate for.
     *
     * @param count
     *   Number of handles to allocate.
     *
     * @returns
     *   Handle to to first allocated handle in range.
     */
    D3D12DescriptorHandle allocate(std::uint32_t frame, std::uint32_t count)
    {
        expect(frame < N, "invalid frame number");
        expect(indices_[frame] + count <= capacity_ * (frame + 1u), "heap too small");

        auto cpu_handle = cpu_start_;
        cpu_handle.ptr += descriptor_size_ * indices_[frame];

        auto gpu_handle = gpu_start_;
        gpu_handle.ptr += descriptor_size_ * indices_[frame];

        indices_[frame] += count;

        return {cpu_handle, gpu_handle};
    }
    /**
     * Get cpu handle of first descriptor in heap.
     *
     * @returns
     *   First cpu descriptor in heap.
     */
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start() const
    {
        return cpu_start_;
    }

    /**
     * Get gpu handle of first descriptor in heap.
     *
     * @returns
     *   First gpu descriptor in heap.
     */
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start() const
    {
        return gpu_start_;
    }

    /**
     * Get the size of a descriptor handle.
     *
     * @returns
     *   Descriptor handle size.
     */
    std::uint32_t descriptor_size() const
    {
        return descriptor_size_;
    }

    /**
     * Get native handle to heap.
     *
     * @returns
     *   Pointer to heap.
     */
    ID3D12DescriptorHeap *heap() const
    {
        return descriptor_heap_.Get();
    }

    /**
     * Reset the allocation for a frame. This means future calls to allocate could return handles that have been
     * previously allocated.
     *
     * @param frame
     *   Frame number to reset.
     *
     */
    void reset(std::uint32_t frame)
    {
        expect(frame < N, "invalid frame number");

        indices_[frame] = frame * capacity_;
    }

  private:
    /** D3D12 handle to descriptor heap. */
    ::Microsoft::WRL::ComPtr<::ID3D12DescriptorHeap> descriptor_heap_;

    /** First cpu handle. */
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start_;

    /** First gpu handle. */
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start_;

    /** Size of a single descriptor handle. */
    std::uint32_t descriptor_size_;

    /** Current index into each pool. */
    std::array<std::uint32_t, N> indices_;

    /** Maximum number of handles. */
    std::uint32_t capacity_;
};
}
