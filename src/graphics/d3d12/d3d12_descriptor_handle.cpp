////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_descriptor_handle.h"

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

namespace iris
{

D3D12DescriptorHandle::D3D12DescriptorHandle()
    : cpu_handle_()
    , gpu_handle_()
{
    cpu_handle_.ptr = NULL;
    gpu_handle_.ptr = NULL;
}

D3D12DescriptorHandle::D3D12DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle)
    : cpu_handle_(cpu_handle)
    , gpu_handle_()
{
    gpu_handle_.ptr = NULL;
}

D3D12DescriptorHandle::D3D12DescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
    : cpu_handle_(cpu_handle)
    , gpu_handle_(gpu_handle)
{
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHandle::cpu_handle() const
{
    return cpu_handle_;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHandle::gpu_handle() const
{
    return gpu_handle_;
}

D3D12DescriptorHandle::operator bool() const
{
    // only need to check cpu handle as there is no situation where we could
    // have a gpu handle without a cpu handle
    return cpu_handle_.ptr != NULL;
}

}