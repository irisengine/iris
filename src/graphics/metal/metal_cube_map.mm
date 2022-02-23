////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_cube_map.h"

#include <cstddef>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "core/macos/macos_ios_utility.h"
#include "graphics/cube_map.h"

namespace iris
{

MetalCubeMap::MetalCubeMap(
    const DataBuffer &right_data,
    const DataBuffer &left_data,
    const DataBuffer &top_data,
    const DataBuffer &bottom_data,
    const DataBuffer &back_data,
    const DataBuffer &front_data,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t index)
    : CubeMap(index)
    , texture_(nullptr)
{
    ensure(width == height, "cube map image must be square");

    auto *device = iris::core::utility::metal_device();

    const auto texture_descriptor =
        [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm_sRGB
                                                              size:width
                                                         mipmapped:NO];
    texture_ = [device newTextureWithDescriptor:texture_descriptor];

    const std::byte *data_ptrs[] = {
        right_data.data(), left_data.data(), top_data.data(), bottom_data.data(), back_data.data(), front_data.data()};

    // setup region and byte properties of textures
    const auto region = MTLRegionMake2D(0, 0, width, height);
    const auto bytes_per_row = width * 4u;
    const auto bytes_per_image = bytes_per_row * height;

    // set data for each cube face
    for (auto i = 0u; i < 6u; ++i)
    {
        [texture_ replaceRegion:region
                    mipmapLevel:0
                          slice:i
                      withBytes:data_ptrs[i]
                    bytesPerRow:bytes_per_row
                  bytesPerImage:bytes_per_image];
    }
}

id<MTLTexture> MetalCubeMap::handle() const
{
    return texture_;
}

}
