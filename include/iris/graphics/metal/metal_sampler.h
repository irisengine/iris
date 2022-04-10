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

class MetalSampler : public Sampler
{
  public:
    MetalSampler(const SamplerDescriptor &descriptor, std::uint32_t index);
    ~MetalSampler() override = default;

    id<MTLSamplerState> handle() const;

  private:
    id<MTLSamplerState> sampler_;
};

}
