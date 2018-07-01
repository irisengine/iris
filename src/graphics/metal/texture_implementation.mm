#include "metal/texture_implementation.hpp"

#include <any>
#include <cstdint>
#include <vector>

#import <Metal/Metal.h>

#include "exception.hpp"

namespace
{

/**
 * Helper method to convert number of channels into a metal type.
 *
 * @param num_channels
 *   The number of channels in an image.
 *
 * @returns
 *   A metal type representing the number of channels.
 */
MTLPixelFormat channels_to_format(const std::uint32_t num_channels)
{
    auto format = MTLPixelFormatR8Unorm;

    switch(num_channels)
    {
        case 1:
            format = MTLPixelFormatR8Unorm;
            break;
        case 4:
            format = MTLPixelFormatRGBA8Unorm;
            break;
        default:
            throw eng::exception("incorrect number of channels");
    }

    return format;
}

}

namespace eng
{

texture_implementation::texture_implementation(
    const std::vector<std::uint8_t> &data,
    const std::uint32_t height,
    const std::uint32_t width,
    const std::uint32_t num_channels)
{
    const auto format = channels_to_format(num_channels);

    // create metal texture descriptor
    auto *texture_descriptor =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:format
                                                           width:width
                                                          height:height
                                                       mipmapped:YES];

    // get metal device handle
    static const auto *device =
        ::CGDirectDisplayCopyCurrentMetalDevice(::CGMainDisplayID());

    // create new texture
    auto texture = [device newTextureWithDescriptor:texture_descriptor];

    auto region = MTLRegionMake2D(0, 0, width, height);
    const auto bytes_per_row = width * num_channels;

    // set image data for texture
    [texture replaceRegion:region
               mipmapLevel:0
                 withBytes:data.data()
               bytesPerRow:bytes_per_row];

    texture_ = texture;
}

std::any texture_implementation::native_handle() const
{
    return texture_;
}

}

