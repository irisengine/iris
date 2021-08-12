#include "graphics/metal/metal_texture.h"

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/macos/macos_ios_utility.h"
#include "core/resource_loader.h"
#include "core/vector3.h"
#include "graphics/pixel_format.h"
#include "log/log.h"

namespace
{

/**
 * Helper method to convert engine pixel format into an metal enum.
 *
 * @param pixel_format
 *   Format of texture data.
 *
 * @returns
 *   An metal enum representing the pixel format.
 */
MTLPixelFormat forma_to_metal(iris::PixelFormat pixel_format)
{
    auto format = MTLPixelFormatR8Unorm;

    switch (pixel_format)
    {
        case iris::PixelFormat::R: format = MTLPixelFormatR8Unorm; break;
        case iris::PixelFormat::RGBA: format = MTLPixelFormatBGRA8Unorm; break;
        default: throw iris::Exception("unsupported pixel format");
    }

    return format;
}

/**
 * Helper function to create a metal Texture from pixel data.
 *
 * @param data
 *   Raw data of image.
 *
 * @param width
 *   Width of image.
 *
 * @param height
 *   Height of image.
 *
 * @param pixel_format
 *   Format of texture data.
 *
 * @returns
 *   Handle to texture.
 */
id<MTLTexture> create_texture(
    iris::DataBuffer data,
    std::uint32_t width,
    std::uint32_t height,
    iris::PixelFormat pixel_format)
{
    auto *data_ptr = data.data();

    iris::DataBuffer padded{};

    MTLTextureDescriptor *texture_descriptor = nullptr;

    if (pixel_format == iris::PixelFormat::DEPTH)
    {
        texture_descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                         width:width
                                        height:height
                                     mipmapped:NO];
        [texture_descriptor setResourceOptions:MTLResourceStorageModePrivate];
        [texture_descriptor
            setUsage:MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead];
    }
    else
    {
        // annoyingly metal only allows BGRA for render targets
        // rather than adding this to PixelFormat and generally muddying the
        // user api we instead do a byte swap here to convert to BGRA this means
        // that to a user everything is RGBA, but interally to metal all
        // textures are BGRA
        if ((pixel_format == iris::PixelFormat::RGBA) && !data.empty())
        {
            for (auto i = 0u; i < width * height * 4; i += 4)
            {
                std::swap(data_ptr[i], data_ptr[i + 2]);
            }
        }

        const auto format = forma_to_metal(pixel_format);

        // create metal texture descriptor
        texture_descriptor = [MTLTextureDescriptor new];
        texture_descriptor.textureType = MTLTextureType2D;
        texture_descriptor.width = width;
        texture_descriptor.height = height;
        texture_descriptor.pixelFormat = format;
        texture_descriptor.usage =
            MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    }

    auto *device = iris::core::utility::metal_device();

    // create new texture
    auto texture = [device newTextureWithDescriptor:texture_descriptor];

    auto region = MTLRegionMake2D(0, 0, width, height);
    const auto bytes_per_row = width * 4u;

    // special case for empty texture
    if (!data.empty())
    {
        // set image data for texture
        [texture replaceRegion:region
                   mipmapLevel:0
                     withBytes:data_ptr
                   bytesPerRow:bytes_per_row];
    }

    return texture;
}

}

namespace iris
{

MetalTexture::MetalTexture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
    : Texture(data, width, height, pixel_format)
    , texture_()
{
    texture_ = create_texture(data, width, height, pixel_format);

    LOG_ENGINE_INFO("texture", "loaded from data");
}

id<MTLTexture> MetalTexture::handle() const
{
    return texture_;
}

}
