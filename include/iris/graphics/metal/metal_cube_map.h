////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "graphics/cube_map.h"
#include "graphics/sampler.h"

namespace iris
{

/**
 * Implementation of CubeMap for Metal.
 */
class MetalCubeMap : public CubeMap
{
  public:
    /**
     * Construct a new MetalCubeMap.
     *
     * @param right_data
     *   Image data for right face of cube.
     *
     * @param left_data
     *   Image data for left face of cube.
     *
     * @param top_data
     *   Image data for top face of cube.
     *
     * @param bottom_data
     *   Image data for bottom face of cube.
     *
     * @param back_data
     *   Image data for back face of cube.
     *
     * @param front_data
     *   Image data for front face of cube.
     *
     * @param width
     *   Width of each image face.
     *
     * @param height
     *   Height of each image face.
     *
     * @param sampler
     *   Sampler to use for cube map.
     *
     * @param index
     *   Index into the global array of all allocated textures.
     */
    MetalCubeMap(
        const DataBuffer &right_data,
        const DataBuffer &left_data,
        const DataBuffer &top_data,
        const DataBuffer &bottom_data,
        const DataBuffer &back_data,
        const DataBuffer &front_data,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler,
        std::uint32_t index);

    ~MetalCubeMap() override = default;

    /**
     * Get metal handle to texture.
     *
     * @returns
     *   Metal texture handle.
     */
    id<MTLTexture> handle() const;

  private:
    /** Metal texture handle. */
    id<MTLTexture> texture_;
};

}
