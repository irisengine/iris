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
 * This class provides the mechanisms for allocating cpu descriptor heaps and
 * descriptor handles. An instance of this class will pre-allocate a fixed sized
 * pool of descriptors for a given d3d12 heap type, it then provides methods for
 * allocating handles from this pool. This class maintains two types of
 * allocation:
 *  - static : allocated for the lifetime of the class
 *  - dynamic : reserved until reset
 *
 * This allows for handles to be allocated that only live for one render pass
 * (dynamic) or multiple frames (static).
 *
 * Example heap layout:
 *
 *i                Pool of all descriptor handles
 *                  +----------------------+ -.
 *                  | static descriptor 1  |  |
 *                  +----------------------+  |
 *                  | static descriptor 2  |  |
 *                  +----------------------+  |
 *                  |                      |  | Static pool
 *                  |                      |  |
 *                  |                      |  |
 *                  |                      |  |
 *                  |                      |  |
 *               ,- +----------------------+ -'
 *               |  | dynamic descriptor 1 |
 *               |  +----------------------+
 *               |  | dynamic descriptor 2 |
 *               |  +----------------------+
 *               |  | dynamic descriptor 3 |
 *  Dynamic pool |  +----------------------+
 *               |  |                      |
 *               |  |                      |
 *               |  |                      |
 *               |  |                      |
 *               |  |                      |
 *               |  |                      |
 *                '-+----------------------+
 */
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
        bool shader_visible = false);

    /**
     * Allocate a static descriptor from the pool.
     *
     * @returns
     *   A new static descriptor.
     */
    D3D12DescriptorHandle allocate_static();

    /**
     * Allocate a dynamic descriptor from the pool.
     *
     * @returns
     *   A new dynamic descriptor.
     */
    D3D12DescriptorHandle allocate_dynamic();

    /**
     * Get the size of a descriptor handle.
     *
     * @returns
     *   Descriptor handle size.
     */
    std::uint32_t descriptor_size() const;

    /**
     * Reset the dynamic allocation. This means future calls to allocate_dynamic
     * could return handles that have been previously allocated.
     */
    void reset_dynamic();

  private:
    /** D3D12 handle to descriptor heap. */
    ::Microsoft::WRL::ComPtr<::ID3D12DescriptorHeap> descriptor_heap_;

    /** D3D12 handle to start of heap. */
    D3D12_CPU_DESCRIPTOR_HANDLE heap_start_;

    /** Size of a single descriptor handle. */
    std::uint32_t descriptor_size_;

    /** The current index into the static pool. */
    std::uint32_t static_index_;

    /** The current index into the dynamic pool. */
    std::uint32_t dynamic_index_;

    /** Maximum number of static handles. */
    std::uint32_t static_capacity_;

    /** Maximum number of dynamic handles. */
    std::uint32_t dynamic_capacity_;
};

}
