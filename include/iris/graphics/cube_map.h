////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "graphics/sampler.h"

namespace iris
{

/**
 * Abstract class for a cube map i.e. a texture with six slices, one for each face of a cube.
 */
class CubeMap
{
  public:
    /**
     * Construct a new CubeMap.
     *
     * @param sampler
     *   Sampler to use for this cube map.
     *
     * @param index
     *   Index into the global array of all allocated cube maps.
     */
    CubeMap(const Sampler *sampler, std::uint32_t index);
    virtual ~CubeMap() = default;

    /**
     * Get cube map sampler.
     *
     * @returns
     *   Sampler for cube map`.
     */
    const Sampler *sampler() const;

    /**
     * Get index into global array of all allocated cube maps.
     *
     * This is most useful for bindless cube maps as the cube map knows where in the mapped cube map table to find
     * itself.
     *
     * @returns
     *   Index of cube map.
     */
    std::uint32_t index() const;

  private:
    /** Sampler for cube map. */
    const Sampler *sampler_;

    /** Index into the global array of all allocated cube maps. */
    std::uint32_t index_;
};

}
