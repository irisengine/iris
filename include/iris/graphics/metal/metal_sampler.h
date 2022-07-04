////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/sampler.h"

#import <Metal/Metal.h>

namespace iris
{

/**
 * Implementation of Sampler for D3D12.
 */
class MetalSampler : public Sampler
{
  public:
    /**
     * Create a new MetalSampler.
     *
     * @param descriptor
     *   Description of sampler parameters.
     *
     * @param index
     *   Index into the global array of all allocated samplers.
     */
    MetalSampler(const SamplerDescriptor &descriptor, std::uint32_t index);
    ~MetalSampler() override = default;

    /**
     * Get the metal handle for the sampler object.
     *
     * @returns
     *   Metal handle.
     */
    id<MTLSamplerState> handle() const;

  private:
    /** Native metal sampler object. */
    id<MTLSamplerState> sampler_;
};

}
