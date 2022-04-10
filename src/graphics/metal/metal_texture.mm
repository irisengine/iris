////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
#include "graphics/sampler.h"
#include "graphics/texture_usage.h"
#include "graphics/utils.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to create a metal texture descriptor suitable for the texture
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @returns
 *   MTLTextureDescriptor for texture.
 */
MTLTextureDescriptor *image_texture_descriptor(std::uint32_t width, std::uint32_t height)
{
    auto *texture_descriptor = [MTLTextureDescriptor new];
    texture_descriptor.textureType = MTLTextureType2D;
    texture_descriptor.width = width;
    texture_descriptor.height = height;
    texture_descriptor.pixelFormat = MTLPixelFormatRGBA8Unorm_sRGB;
    texture_descriptor.usage = MTLTextureUsageShaderRead;

    return texture_descriptor;
}

/**
 * Helper function to create a metal texture descriptor suitable for the data
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @returns
 *   MTLTextureDescriptor for texture.
 */
MTLTextureDescriptor *data_texture_descriptor(std::uint32_t width, std::uint32_t height)
{
    auto *texture_descriptor = [MTLTextureDescriptor new];
    texture_descriptor.textureType = MTLTextureType2D;
    texture_descriptor.width = width;
    texture_descriptor.height = height;
    texture_descriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
    texture_descriptor.usage = MTLTextureUsageShaderRead;

    return texture_descriptor;
}

/**
 * Helper function to create a metal texture descriptor suitable for the render
 * target usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @returns
 *   MTLTextureDescriptor for texture.
 */
MTLTextureDescriptor *render_target_texture_descriptor(std::uint32_t width, std::uint32_t height)
{
    auto *texture_descriptor = [MTLTextureDescriptor new];
    texture_descriptor.textureType = MTLTextureType2D;
    texture_descriptor.width = width;
    texture_descriptor.height = height;
    texture_descriptor.pixelFormat = MTLPixelFormatRGBA16Float;
    texture_descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;

    return texture_descriptor;
}

/**
 * Helper function to create a metal texture descriptor suitable for the depth
 * usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @returns
 *   MTLTextureDescriptor for texture.
 */
MTLTextureDescriptor *depth_texture_descriptor(std::uint32_t width, std::uint32_t height)
{
    auto *texture_descriptor = [MTLTextureDescriptor new];
    texture_descriptor.textureType = MTLTextureType2D;
    texture_descriptor.width = width;
    texture_descriptor.height = height;
    texture_descriptor.pixelFormat = MTLPixelFormatDepth32Float;
    texture_descriptor.resourceOptions = MTLResourceStorageModePrivate;
    texture_descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;

    return texture_descriptor;
}

}

namespace iris
{

MetalTexture::MetalTexture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    const Sampler *sampler,
    TextureUsage usage,
    std::uint32_t index)
    : Texture(data, width, height, sampler, usage, index)
    , texture_()
{
    auto *device = iris::core::utility::metal_device();

    MTLTextureDescriptor *texture_descriptor = nullptr;

    switch (usage)
    {
        case iris::TextureUsage::IMAGE: texture_descriptor = image_texture_descriptor(width, height); break;
        case iris::TextureUsage::DATA: texture_descriptor = data_texture_descriptor(width, height); break;
        case iris::TextureUsage::RENDER_TARGET:
            texture_descriptor = render_target_texture_descriptor(width, height);
            break;
        case iris::TextureUsage::DEPTH: texture_descriptor = depth_texture_descriptor(width, height); break;
        default: throw iris::Exception("unknown texture usage");
    }

    std::vector<MipLevelData> mip_levels{{.data = data, .width = width_, .height = height_}};

    if (sampler->descriptor().uses_mips)
    {
        mip_levels = generate_mip_maps(mip_levels.front());
    }

    texture_descriptor.mipmapLevelCount = mip_levels.size();

    // create new texture
    texture_ = [device newTextureWithDescriptor:texture_descriptor];

    // set data if it was supplied
    if (!data.empty())
    {
        auto index = 0u;
        for (const auto &[mip_data, mip_width, mip_height] : mip_levels)
        {
            auto region = MTLRegionMake2D(0, 0, mip_width, mip_height);
            const auto bytes_per_row = mip_width * 4u;

            // set image data for texture
            [texture_ replaceRegion:region mipmapLevel:index withBytes:mip_data.data() bytesPerRow:bytes_per_row];
            ++index;
        }
    }

    LOG_ENGINE_INFO("texture", "loaded from data");
}

id<MTLTexture> MetalTexture::handle() const
{
    return texture_;
}

}
