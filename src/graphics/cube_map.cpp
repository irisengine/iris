////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/cube_map.h"

#include <cstdint>

#include "graphics/sampler.h"

namespace iris
{

CubeMap::CubeMap(const Sampler *sampler, std::uint32_t index)
    : sampler_(sampler)
    , index_(index)
{
}

std::uint32_t CubeMap::index() const
{
    return index_;
}

const Sampler *CubeMap::sampler() const
{
    return sampler_;
}

}
