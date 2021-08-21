#pragma once

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

namespace iris
{

/**
 * This class encapsulates a d3d12 descriptor handle, which is a unique handle
 * to an opaque block of data that fully describes an object to the gpu.
 *
 * For more information see:
 * https://docs.microsoft.com/en-us/windows/win32/direct3d12/descriptors-overview
 */
class D3D12DescriptorHandle
{
  public:
    /**
     * Construct an empty (or null) D3D12DescriptorHandle.
     */
    D3D12DescriptorHandle();

    /**
     * Construct a D3D12DescriptorHandle with a cpu handle.
     *
     * @param cpu_handle
     *   CPU handle for desciptor.
     */
    D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle);

    /**
     * Construct a D3D12DescriptorHandle with a cpu and gpu handle.
     *
     * @param cpu_handle
     *   CPU handle for descriptor.
     *
     * @param gpu_handle
     *   GPU handle for descriptor.
     */
    D3D12DescriptorHandle(
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
        D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle);

    /**
     * Get cpu handle, maybe null.
     *
     * @returns
     *   CPU handle.
     */
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle() const;

    /**
     * Get gpu handle, maybe null.
     *
     * @returns
     *   GPU handle.
     */
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle() const;

    /**
     * Get if this object has a descriptor.
     *
     * @returns
     *   True if object has a descriptor, false otherwise.
     */
    explicit operator bool() const;

  private:
    /** CPU descriptor handle, maybe null. */
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_;

    /** GPU descriptor handle, maybe null. */
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_;
};

}
