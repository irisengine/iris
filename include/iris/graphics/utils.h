////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

#include "core/data_buffer.h"

namespace iris
{

/**
 * Struct encapsulating data at a mip level.
 */
struct MipLevelData
{
    /** Image data. */
    DataBuffer data;

    /** Width of mip level (in pixels). */
    std::uint32_t width;

    /** Height of mip level (in pixels). */
    std::uint32_t height;
};

/**
 * Generate mipmaps from a starting level down to a 1x1 pixel.
 *
 * Note that the supplied level will always be the first entry in the returned collection.
 *
 * The algorithm used is a simple downsize by averaging the values of a 2x2 kernel.
 *
 * @param start_level
 *   The first level of the mipmap chain, this is assumed to be the original full sized texture.
 *
 * @returns
 *   Collection of MipLevelData objects where each subsequent entry is half the size of the preceding one.
 */
std::vector<MipLevelData> generate_mip_maps(const MipLevelData &start_level);

}
