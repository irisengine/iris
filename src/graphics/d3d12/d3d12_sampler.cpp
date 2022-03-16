////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_sampler.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/sampler.h"

namespace
{

/**
 * Helper function to convert an engine address mode to a D3D12 address mode.
 *
 * @param address_mode
 *   Engine address mode.
 *
 * @returns
 *   D3D12 address mode.
 */
D3D12_TEXTURE_ADDRESS_MODE to_d3d12(iris::SamplerAddressMode address_mode)
{
    switch (address_mode)
    {
        case iris::SamplerAddressMode::REPEAT: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case iris::SamplerAddressMode::MIRROR: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case iris::SamplerAddressMode::CLAMP_TO_EDGE: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case iris::SamplerAddressMode::CLAMP_TO_BORDER: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        default: throw iris::Exception("unknown address mode");
    }
}

/**
 * Helper method to convert an engine filter to a D3D12 filter.
 *
 * @param descriptor
 *   SamplerDescriptor.
 *
 * @returns
 *   D3D12 filter.
 */
D3D12_FILTER to_d3d12(const iris::SamplerDescriptor &descriptor)
{
    if ((descriptor.minification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.magnification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.mip_filter == iris::SamplerFilter::NEAREST))
    {
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.magnification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.mip_filter == iris::SamplerFilter::LINEAR))
    {
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.magnification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.mip_filter == iris::SamplerFilter::NEAREST))
    {
        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.magnification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.mip_filter == iris::SamplerFilter::LINEAR))
    {
        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.magnification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.mip_filter == iris::SamplerFilter::NEAREST))
    {
        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.magnification_filter == iris::SamplerFilter::NEAREST) &&
        (descriptor.mip_filter == iris::SamplerFilter::LINEAR))
    {
        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.magnification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.mip_filter == iris::SamplerFilter::NEAREST))
    {
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    }
    else if (
        (descriptor.minification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.magnification_filter == iris::SamplerFilter::LINEAR) &&
        (descriptor.mip_filter == iris::SamplerFilter::LINEAR))
    {
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    }

    throw iris::Exception("unknown filter combination");
}

}

namespace iris
{

D3D12Sampler::D3D12Sampler(const SamplerDescriptor &descriptor, std::uint32_t index)
    : Sampler(descriptor, index)
    , descriptor_(D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).allocate_static())
{
    const D3D12_SAMPLER_DESC sampler{
        .Filter = to_d3d12(descriptor),
        .AddressU = to_d3d12(descriptor.s_address_mode),
        .AddressV = to_d3d12(descriptor.t_address_mode),
        .AddressW = to_d3d12(descriptor.r_address_mode),
        .MipLODBias = 0,
        .MaxAnisotropy = 0,
        .ComparisonFunc = D3D12_COMPARISON_FUNC_LESS,
        .BorderColor =
            {descriptor.border_colour.r,
             descriptor.border_colour.g,
             descriptor.border_colour.b,
             descriptor.border_colour.a},
        .MinLOD = 0.0f,
        .MaxLOD = D3D12_FLOAT32_MAX};

    D3D12Context::device()->CreateSampler(&sampler, descriptor_.cpu_handle());
}

D3D12Sampler::~D3D12Sampler()
{
    D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).release_static(descriptor_);
}

D3D12DescriptorHandle D3D12Sampler::handle() const
{
    return descriptor_;
}

}
