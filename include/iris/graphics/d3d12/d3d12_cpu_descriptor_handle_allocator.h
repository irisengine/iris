////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cstdint>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "log/log.h"

namespace iris
{

/**
 * This class provides the mechanisms for allocating cpu descriptor heaps and descriptor handles. An instance of this
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
 */
template <std::uint32_t N>
class D3D12CPUDescriptorHandleAllocator
{
  public:
    /**
     * Construct a new D3D12CPUDescriptorHandleAllocator.
     *
     * @param type
     *   D3D12 heap type to create.
     *
     * @param num_descriptors
     *   Number of descriptors to create in heap (static + dynamic).
     *
     * @param static_size
     *   The number of descriptors to reserve for static descriptors.
     *
     * @param shader_visible
     *   Flag indicating whether the allocated descriptors should be visible to
     *   the gpu or just the cpu.
     */
    D3D12CPUDescriptorHandleAllocator(
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        std::uint32_t num_descriptors,
        std::uint32_t static_size,
        bool shader_visible = false)
        : descriptor_heap_(nullptr)
        , heap_start_()
        , descriptor_size_(0u)
        , static_index_(0u)
        , dynamic_indices_()
        , static_capacity_(static_size)
        , dynamic_capacity_((num_descriptors - static_size) / N)
        , static_free_list_()
    {
        auto *device = D3D12Context::device();

        // setup heap description
        D3D12_DESCRIPTOR_HEAP_DESC heap_description;
        heap_description.NumDescriptors = num_descriptors;
        heap_description.Type = type;
        heap_description.Flags =
            shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heap_description.NodeMask = 0;

        // create heap
        expect(
            device->CreateDescriptorHeap(&heap_description, IID_PPV_ARGS(&descriptor_heap_)) == S_OK,
            "could not create descriptor heap");

        descriptor_size_ = device->GetDescriptorHandleIncrementSize(type);

        heap_start_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();

        // set indices for dynamic pools
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
    D3D12DescriptorHandle allocate_static()
    {
        auto index = static_index_;

        // try and get an index from the free list before using the next available one
        if (!static_free_list_.empty())
        {
            index = static_free_list_.back();
            static_free_list_.pop_back();
        }
        else
        {
            expect(static_index_ < static_capacity_, "heap too small");
            ++static_index_;
        }

        // get next free descriptor form static pool
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap_start_;
        cpu_handle.ptr += descriptor_size_ * index;

        return {cpu_handle};
    }
    /**
     * Allocate a dynamic descriptor from the pool.
     *
     * @returns
     *   A new dynamic descriptor.
     */
    D3D12DescriptorHandle allocate_dynamic(std::uint32_t frame)
    {
        expect(frame < N, "invalid frame number");
        expect(dynamic_indices_[frame] < static_capacity_ + ((frame + 1u) * dynamic_capacity_), "heap too small");

        // get next free descriptor form dynamic pool
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap_start_;
        cpu_handle.ptr += descriptor_size_ * (static_capacity_ + dynamic_indices_[frame]);

        ++dynamic_indices_[frame];

        return {cpu_handle};
    }

    /**
     * Release a statically allocate handle.
     *
     * @param handle
     *   Handle to release.
     */
    void release_static(const D3D12DescriptorHandle &handle)
    {
        // recompute index from ptr
        const auto ptr = handle.cpu_handle().ptr;
        const auto index = (ptr - heap_start_.ptr) / descriptor_size_;

        static_free_list_.emplace_back(static_cast<std::uint32_t>(index));
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
     * Reset the dynamic allocation for a given frame. This means future calls to allocate_dynamic could return handles
     * that have been previously allocated.
     *
     * @param frame
     *   Frame number to reset.
     */
    void reset_dynamic(std::uint32_t frame)
    {
        expect(frame < N, "invalid frame number");

        dynamic_indices_[frame] = static_capacity_ + (frame * dynamic_capacity_);
    }

  private:
    /** D3D12 handle to descriptor heap. */
    ::Microsoft::WRL::ComPtr<::ID3D12DescriptorHeap> descriptor_heap_;

    /** D3D12 handle to start of heap. */
    D3D12_CPU_DESCRIPTOR_HANDLE heap_start_;

    /** Size of a single descriptor handle. */
    std::uint32_t descriptor_size_;

    /** The current index into the static pool. */
    std::uint32_t static_index_;

    /** The current index into each dynamic pool. */
    std::array<std::uint32_t, N> dynamic_indices_;

    /** Maximum number of static handles. */
    std::uint32_t static_capacity_;

    /** Maximum number of dynamic handles in a frame. */
    std::uint32_t dynamic_capacity_;

    /** Free list of released handles. */
    std::vector<std::uint32_t> static_free_list_;
};

}
