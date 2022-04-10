////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_sampler.h"

#include "core/colour.h"
#include "core/error_handling.h"
#include "core/macos/macos_ios_utility.h"
#include "graphics/sampler.h"

#import <Metal/Metal.h>

namespace
{

/**
 * Helper function to convert an engine address mode to a metal address mode.
 *
 * @param address_mode
 *   Engine address mode.
 *
 * @returns
 *   Metal address mode.
 */
MTLSamplerAddressMode to_metal(iris::SamplerAddressMode address_mode)
{
    switch (address_mode)
    {
        case iris::SamplerAddressMode::REPEAT: return MTLSamplerAddressModeRepeat;
        case iris::SamplerAddressMode::MIRROR: return MTLSamplerAddressModeMirrorRepeat;
        case iris::SamplerAddressMode::CLAMP_TO_EDGE: return MTLSamplerAddressModeClampToEdge;
        case iris::SamplerAddressMode::CLAMP_TO_BORDER: return MTLSamplerAddressModeClampToBorderColor;
        default: throw iris::Exception("unknown address mode");
    }
}

/**
 * Helper method to convert an engine filter to a metal filter.
 *
 * @param filter
 *   Engine filter.
 *
 * @returns
 *   Metal filter.
 */
MTLSamplerMinMagFilter to_metal(iris::SamplerFilter filter)
{
    switch (filter)
    {
        case iris::SamplerFilter::NEAREST: return MTLSamplerMinMagFilterNearest;
        case iris::SamplerFilter::LINEAR: return MTLSamplerMinMagFilterLinear;
        default: throw iris::Exception("unknown filter");
    }
}

/**
 * Helper method to convert an engine mip filter to a metal filter.
 *
 * @param filter
 *   Engine filter.
 *
 * @param uses_mips
 *   Flag indicating if mips are used.
 *
 * @returns
 *   Metal filter.
 */
MTLSamplerMipFilter to_metal([[maybe_unused]] iris::SamplerFilter filter, [[maybe_unused]] bool uses_mips)
{
    if (!uses_mips)
    {
        return MTLSamplerMipFilterNotMipmapped;
    }

    switch (filter)
    {
        case iris::SamplerFilter::NEAREST: return MTLSamplerMipFilterNearest;
        case iris::SamplerFilter::LINEAR: return MTLSamplerMipFilterLinear;
        default: throw iris::Exception("unknown filter");
    }
}

MTLSamplerBorderColor to_metal(const iris::Colour &colour)
{
    if (colour == iris::Colour{0.0f, 0.0f, 0.0f, 0.0f})
    {
        return MTLSamplerBorderColorTransparentBlack;
    }
    else if (colour == iris::Colour{0.0f, 0.0f, 0.0f, 1.0f})
    {
        return MTLSamplerBorderColorOpaqueBlack;
    }
    else if (colour == iris::Colour{1.0f, 1.0f, 1.0f, 1.0f})
    {
        return MTLSamplerBorderColorOpaqueWhite;
    }
    else
    {
        throw iris::Exception("unsupported border colour");
    }
}

}

namespace iris
{

MetalSampler::MetalSampler(const SamplerDescriptor &descriptor, std::uint32_t index)
    : Sampler(descriptor, index)
    , sampler_()
{
    const auto *device = iris::core::utility::metal_device();

    auto *metal_sampler_descriptor = [MTLSamplerDescriptor new];
    metal_sampler_descriptor.sAddressMode = to_metal(descriptor.s_address_mode);
    metal_sampler_descriptor.tAddressMode = to_metal(descriptor.t_address_mode);
    metal_sampler_descriptor.rAddressMode = to_metal(descriptor.r_address_mode);
    metal_sampler_descriptor.borderColor = to_metal(descriptor.border_colour);
    metal_sampler_descriptor.minFilter = to_metal(descriptor.minification_filter);
    metal_sampler_descriptor.magFilter = to_metal(descriptor.magnification_filter);
    metal_sampler_descriptor.mipFilter = to_metal(descriptor.mip_filter, descriptor.uses_mips);
    metal_sampler_descriptor.supportArgumentBuffers = YES;
    metal_sampler_descriptor.compareFunction = MTLCompareFunctionNever;
    sampler_ = [device newSamplerStateWithDescriptor:metal_sampler_descriptor];
}

id<MTLSamplerState> MetalSampler::handle() const
{
    return sampler_;
}

}
