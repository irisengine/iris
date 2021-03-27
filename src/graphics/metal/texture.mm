#include "graphics/texture.h"

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/vector3.h"
#include "graphics/pixel_format.h"
#include "log/log.h"
#include "core/macos/macos_ios_utility.h"
#include "core/resource_loader.h"

namespace
{

/**
 * In order to have a pool of reusable texture ids we maintain a static stack.
 * This will be lazily populated by the first loaded texture, from there any
 * deleted texture will return its id to this pool.
 */
static std::stack<std::uint32_t> id_pool;

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

    switch(pixel_format)
    {
        case iris::PixelFormat::R:
            format = MTLPixelFormatR8Unorm;
            break;
        case iris::PixelFormat::RGBA:
            format = MTLPixelFormatRGBA8Unorm;
            break;
        default:
            throw iris::Exception("unsupported pixel format");
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
std::tuple<id<MTLTexture>, std::uint32_t> create_texture(
    const iris::DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    iris::PixelFormat pixel_format)
{
    if (id_pool.empty())
    {
        throw iris::Exception("maximum active textures");
    }

    auto *data_ptr = data.data();

    iris::DataBuffer padded{};

    MTLTextureDescriptor *texture_descriptor = nullptr;

    if (pixel_format == iris::PixelFormat::DEPTH)
    {
        texture_descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:width
                                                          height:height
                                                       mipmapped:NO];
        [texture_descriptor setResourceOptions:MTLResourceStorageModePrivate];
        [texture_descriptor setUsage:MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead];

    }
    else
    {
        // metal does not support RGB nativley, so we extend the data to have
        // a fouth component (alpha - always 1)
        if (pixel_format == iris::PixelFormat::RGB)
        {
            pixel_format = iris::PixelFormat::RGBA;
            padded = iris::DataBuffer(width * height * 4u, static_cast<std::byte>(0xFF));
            data_ptr = padded.data();

            auto *dst = padded.data();
            auto *src = data.data();

            for (auto i = 0u; i < width * height; ++i)
            {
                std::memcpy(dst, src, 3);
                dst += 4;
                src += 3;
            }

            data_ptr = padded.data();
        }

        const auto format = forma_to_metal(pixel_format);

        // create metal Texture descriptor
        texture_descriptor = [MTLTextureDescriptor new];
        texture_descriptor.textureType = MTLTextureType2D;
        texture_descriptor.width = width;
        texture_descriptor.height = height;
        texture_descriptor.pixelFormat = format;
        texture_descriptor.usage = MTLTextureUsageRenderTarget |
                                   MTLTextureUsageShaderRead;
    }
    
    // get metal device handle
    auto *device = iris::core::utility::metal_device();

    // create new texture
    auto texture = [device newTextureWithDescriptor:texture_descriptor];

    auto region = MTLRegionMake2D(0, 0, width, height);
    const auto bytes_per_row = width * 4u;

    // set image data for texture
    [texture replaceRegion:region
               mipmapLevel:0
                 withBytes:data_ptr
               bytesPerRow:bytes_per_row];

    const auto id = id_pool.top();
    id_pool.pop();

    return {texture, id};
}

}

namespace iris
{

struct Texture::implementation
{
    id<MTLTexture> texture;
    std::uint32_t texture_id;
};

Texture::Texture()
    : Texture(DataBuffer(4, static_cast<std::byte>(0xFF)), 1u, 1u, PixelFormat::RGBA)
{
}

Texture::Texture(
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
    : Texture({}, width, height, pixel_format)
{
}

Texture::Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
    : data_(data),
      width_(width),
      height_(height),
      flip_(false),
      impl_(std::make_unique<implementation>())
{
    // populate id pool (this only happens on the first texture creation)
    static auto once = false;
    if (!once)
    {
        for (auto i = 79; i >= 0; --i)
        {
            id_pool.emplace(i);
        }

        once = true;
    }

    const auto [texture, texture_id] = create_texture(data, width, height, pixel_format);
    impl_->texture = texture;
    impl_->texture_id = texture_id;

    LOG_ENGINE_INFO("texture", "loaded from data");
}

Texture::~Texture()
{
    id_pool.emplace(impl_->texture_id);
}

Texture::Texture(Texture&&) = default;
Texture& Texture::operator=(Texture&&) = default;

DataBuffer Texture::data() const
{
    return data_;
}

std::uint32_t Texture::width() const
{
    return width_;
}

std::uint32_t Texture::height() const
{
    return height_;
}

std::any Texture::native_handle() const
{
    return impl_->texture;
}

std::uint32_t Texture::texture_id() const
{
    return impl_->texture_id;
}

bool Texture::flip() const
{
    return flip_;
}

void Texture::set_flip(bool flip)
{
    flip_ = flip;
}

}

