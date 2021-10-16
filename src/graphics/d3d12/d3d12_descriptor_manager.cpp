////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_descriptor_manager.h"

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "graphics/d3d12/d3d12_cpu_descriptor_handle_allocator.h"
#include "graphics/d3d12/d3d12_gpu_descriptor_handle_allocator.h"

namespace iris
{

D3D12CPUDescriptorHandleAllocator &D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    return instance().cpu_allocator_impl(type);
}

D3D12GPUDescriptorHandleAllocator &D3D12DescriptorManager::gpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    return instance().gpu_allocator_impl(type);
}

D3D12DescriptorManager::D3D12DescriptorManager()
    : cpu_allocators_()
    , gpu_allocators_()
{
    cpu_allocators_.insert(
        {D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, {D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2048u, 100u}});
    cpu_allocators_.insert({D3D12_DESCRIPTOR_HEAP_TYPE_RTV, {D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024u, 100u}});
    cpu_allocators_.insert({D3D12_DESCRIPTOR_HEAP_TYPE_DSV, {D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1024u, 100u}});
    gpu_allocators_.insert({D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, {D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10240u}});
}

D3D12DescriptorManager &D3D12DescriptorManager::instance()
{
    static D3D12DescriptorManager dm{};
    return dm;
}

D3D12CPUDescriptorHandleAllocator &D3D12DescriptorManager::cpu_allocator_impl(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    return cpu_allocators_.at(type);
}

D3D12GPUDescriptorHandleAllocator &D3D12DescriptorManager::gpu_allocator_impl(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    return gpu_allocators_.at(type);
}

}
