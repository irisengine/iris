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
 * allocating handles from this pool. This class maintains two types of allocation:
 *  - static : allocated for the lifetime of the class
 *  - dynamic : reserved until reset
 *
 * This allows for handles to be allocated that only live for one render pass (dynamic) or multiple frames (static).
 *
 * The dynamic section is split into N sections, one for each frame in the swap chain. This allows a frame to
 * independently manage its own handles.
 *
 * Example heap layout:
 *
 *                Pool of all descriptor handles
 *               .- +----------------------+
 *               |  | static descriptor 1  |
 *               |  +----------------------+
 *               |  | static descriptor 2  |
 *               |  +----------------------+
 *  Static pool  |  |                      |
 *               |  |                      |
 *               |  |                      |
 *               |  |                      |
 *               |  |                      |
 *               +- +----------------------+ -.
 *               |  | dynamic descriptor 1 |  |
 *               |  +----------------------+  |
 *               |  | dynamic descriptor 2 |  |
 *               |  +----------------------+  | Frame 1
 *               |  | dynamic descriptor 3 |  |
 *               |  +----------------------+  |
 *               |  |          ...         |  |
 *               |  +----------------------+ -+
 *               |  | dynamic descriptor 1 |  |
 *               |  +----------------------+  |
 *  Dynamic pool |  | dynamic descriptor 2 |  |
 *               |  +----------------------+  | Frame 2
 *               |  | dynamic descriptor 3 |  |
 *               |  +----------------------+  |
 *               |  |          ...         |  |
 *               |  +----------------------+ -+
 *               |  | dynamic descriptor 1 |  |
 *               |  +----------------------+  |
 *               |  | dynamic descriptor 2 |  |
 *               |  +----------------------+  | Frame 3
 *               |  | dynamic descriptor 3 |  |
 *               |  +----------------------+  |
 *               |  |         ...          |  |
 *                '-+----------------------+ -'
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
    D3D12GPUDescriptorHandleAllocator(
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        std::uint32_t num_descriptors,
        std::uint32_t static_size)
        : descriptor_heap_(nullptr)
        , cpu_start_()
        , gpu_start_()
        , descriptor_size_(0u)
        , static_index_(0u)
        , dynamic_indices_()
        , static_capacity_(static_size)
        , dynamic_capacity_((num_descriptors - static_size) / N)
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
            dynamic_indices_[i] = static_size + (i * dynamic_capacity_);
        }
    }

    /**
     * Allocate a static descriptor from the pool.
     *
     * @returns
     *   A new static descriptor.
     */
    D3D12DescriptorHandle allocate_static(std::uint32_t count)
    {
        expect(static_index_ + count < static_capacity_, "heap too small");

        auto cpu_handle = cpu_start_;
        cpu_handle.ptr += descriptor_size_ * static_index_;

        auto gpu_handle = gpu_start_;
        gpu_handle.ptr += descriptor_size_ * static_index_;

        static_index_ += count;

        return {cpu_handle, gpu_handle};
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
    D3D12DescriptorHandle allocate_dynamic(std::uint32_t frame, std::uint32_t count)
    {
        expect(frame < N, "invalid frame number");
        expect(
            dynamic_indices_[frame] + count < static_capacity_ + ((frame + 1u) * dynamic_capacity_), "heap too small");

        auto cpu_handle = cpu_start_;
        cpu_handle.ptr += descriptor_size_ * (static_capacity_ + dynamic_indices_[frame]);

        auto gpu_handle = gpu_start_;
        gpu_handle.ptr += descriptor_size_ * (static_capacity_ + dynamic_indices_[frame]);

        dynamic_indices_[frame] += count;

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
    void reset_dynamic(std::uint32_t frame)
    {
        expect(frame < N, "invalid frame number");

        dynamic_indices_[frame] = static_capacity_ + (frame * dynamic_capacity_);
    }

    void reset_static()
    {
        static_index_ = 0u;
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

    /** The current index into the static pool. */
    std::uint32_t static_index_;

    /** Current index into each pool. */
    std::array<std::uint32_t, N> dynamic_indices_;

    /** Maximum number of static handles. */
    std::uint32_t static_capacity_;

    /** Maximum number of handles. */
    std::uint32_t dynamic_capacity_;
};
}
