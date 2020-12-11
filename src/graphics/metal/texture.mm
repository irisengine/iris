#include "graphics/texture.h"

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#import <Metal/Metal.h>

#include "core/exception.h"
#include "log/log.h"
#include "platform/macos/macos_ios_utility.h"
#include "platform/resource_loader.h"

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
            throw iris::Exception("incorrect number of channels");
    }

    return format;
}

/**
 * Helper function to create an metal Texture from data.
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
 * @param num_channels
 *   Number of channels.
 *
 * @returns
 *   Handle to texture.
 */
std::tuple<id<MTLTexture>, std::uint32_t> create_texture(
    const std::vector<std::uint8_t> &data,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t num_channels)
{
    // sanity check we have enough data
    const auto expected_size = width * height * num_channels;
    if(data.size() != expected_size)
    {
        throw iris::Exception("incorrect data size");
    }

    const auto format = channels_to_format(num_channels);

    // create metal Texture descriptor
    auto *texture_descriptor =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:format
                                                           width:width
                                                          height:height
                                                       mipmapped:NO];
    
    // get metal device handle
    auto *device = iris::platform::utility::metal_device();

    // create new texture
    auto texture = [device newTextureWithDescriptor:texture_descriptor];

    auto region = MTLRegionMake2D(0, 0, width, height);
    const auto bytes_per_row = width * num_channels;

    // set image data for texture
    [texture replaceRegion:region
               mipmapLevel:0
                 withBytes:data_ptr
               bytesPerRow:bytes_per_row];

    static std::uint32_t counter = 0u;

    return {texture, counter++};
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Texture::implementation
{
    id<MTLTexture> texture;

    std::uint32_t texture_id;
};


Texture::Texture(
    const std::vector<std::uint8_t> &data,
    const std::uint32_t width,
    const std::uint32_t height,
    const std::uint32_t num_channels)
    : data_(data),
      width_(width),
      height_(height),
      num_channels_(num_channels),
      flip_(false),
      impl_(std::make_unique<implementation>())
{
    const auto [texture, texture_id] = create_texture(data, width, height, num_channels);
    impl_->texture = texture;
    impl_->texture_id = texture_id;

    LOG_ENGINE_INFO("texture", "loaded from data");
}

/** Default. */
Texture::~Texture() = default;
Texture::Texture(Texture&&) = default;
Texture& Texture::operator=(Texture&&) = default;

std::vector<std::uint8_t> Texture::data() const
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

std::uint32_t Texture::num_channels() const
{
    return num_channels_;
}

std::any Texture::native_handle() const
{
    return impl_->texture;
}

std::uint32_t Texture::texture_id() const
{
    return impl_->texture_id;
}

Texture Texture::blank()
{
    return{ { 0xFF, 0xFF, 0xFF, 0xFF }, 1u, 1u, 4u };
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

