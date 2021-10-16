////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "graphics/d3d12/d3d12_descriptor_handle.h"

namespace iris
{

/**
 * This class provides the mechanisms for allocating gpu descriptor heaps
 * and descriptor handles. An instance of this class will pre-allocate a
 * fixed sized pool of descriptors for a given d3d12 heap type, it then
 * provides methods for allocating handles from this pool.
 */
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
     *   Number of descriptors to create in heap (static + dynamic).
     *
     */
    D3D12GPUDescriptorHandleAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, std::uint32_t num_descriptors);

    /**
     * Allocate a contiguous range of descriptor handles.
     *
     * @param count
     *   Number of handles to allocate.
     *
     * @returns
     *   Handle to to first allocated handle in range.
     */
    D3D12DescriptorHandle allocate(std::uint32_t count);

    /**
     * Get cpu handle of first descriptor in heap.
     *
     * @returns
     *   First cpu descriptor in heap.
     */
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start() const;

    /**
     * Get gpu handle of first descriptor in heap.
     *
     * @returns
     *   First gpu descriptor in heap.
     */
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start() const;

    /**
     * Get the size of a descriptor handle.
     *
     * @returns
     *   Descriptor handle size.
     */
    std::uint32_t descriptor_size() const;

    /**
     * Get native handle to heap.
     *
     * @returns
     *   Pointer to heap.
     */
    ID3D12DescriptorHeap *heap() const;

    /**
     * Reset the allocation. This means future calls to allocate could
     * return handles that have been previously allocated.
     */
    void reset();

  private:
    /** D3D12 handle to descriptor heap. */
    ::Microsoft::WRL::ComPtr<::ID3D12DescriptorHeap> descriptor_heap_;

    /** First cpu handle. */
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start_;

    /** First gpu handle. */
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start_;

    /** Size of a single descriptor handle. */
    std::uint32_t descriptor_size_;

    /** Current index into the pool. */
    std::uint32_t index_;

    /** Maximum number of handles. */
    std::uint32_t capacity_;
};
}
