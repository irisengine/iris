////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <unordered_map>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "graphics/d3d12/d3d12_cpu_descriptor_handle_allocator.h"
#include "graphics/d3d12/d3d12_gpu_descriptor_handle_allocator.h"

namespace iris
{

/**
 * This class provides singleton access to heap allocators. It manages a
 * separate heap for various different d3d12 heap types.
 */
class D3D12DescriptorManager
{
  public:
    /**
     * Get the cpu allocator for a given heap type.
     *
     * @param type
     *   D3D12 heap type to get allocator for.
     *
     * @returns
     *   Heap allocator for provided type.
     */
    static D3D12CPUDescriptorHandleAllocator<3u> &cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE type);

    /**
     * Get the gpu allocator for a given heap type.
     *
     * @param type
     *   D3D12 heap type to get allocator for.
     *
     * @returns
     *   Heap allocator for provided type.
     */
    static D3D12GPUDescriptorHandleAllocator<3u> &gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE type);

  private:
    // private to force access through above public static methods
    D3D12DescriptorManager();
    static D3D12DescriptorManager &instance();

    // these are the member function equivalents of the above static methods
    // see their docs for details

    D3D12CPUDescriptorHandleAllocator<3u> &cpu_allocator_impl(D3D12_DESCRIPTOR_HEAP_TYPE type);

    D3D12GPUDescriptorHandleAllocator<3u> &gpu_allocator_impl(D3D12_DESCRIPTOR_HEAP_TYPE type);

    /** Map of D3D12 heap type to heap allocator. */
    std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, D3D12CPUDescriptorHandleAllocator<3u>> cpu_allocators_;

    /** Map of D3D12 heap type to heap allocator. */
    std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, D3D12GPUDescriptorHandleAllocator<3u>> gpu_allocators_;
};

}
