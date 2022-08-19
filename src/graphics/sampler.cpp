////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/sampler.h"

#include <cstdint>

namespace iris
{

Sampler::Sampler(const SamplerDescriptor &descriptor, std::uint32_t index)
    : descriptor_(descriptor)
    , index_(index)
{
}

SamplerDescriptor Sampler::descriptor() const
{
    return descriptor_;
}

std::uint32_t Sampler::index() const
{
    return index_;
}

}
