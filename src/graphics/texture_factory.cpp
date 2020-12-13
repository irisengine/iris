#include "graphics/texture_factory.h"

#include <cstdint>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "core/exception.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"
#include "platform/resource_loader.h"

namespace
{

// static cache of loaded textures
static std::map<std::string, std::unique_ptr<iris::Texture>> cache;

static std::uint32_t counter = 0u;

/**
 * Load an image from a data buffer.
 *
 * @param data
 *   Image data.
 *
 * @returns
 *   Tuple of <data, width, height, number of channels>.
 */
std::tuple<
    std::vector<std::uint8_t>,
    std::uint32_t,
    std::uint32_t,
    std::uint32_t>
parse_image(const std::vector<std::uint8_t> &data)
{
    int width = 0;
    int height = 0;
    int num_channels = 0;

    // ensure that images are flipped along the y axis when loaded, this is so
    // they work with what the graphics api treats as the origin
    ::stbi_set_flip_vertically_on_load(true);

    // load image using stb library
    std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)> raw_data(
        ::stbi_load_from_memory(
            data.data(),
            static_cast<int>(data.size()),
            &width,
            &height,
            &num_channels,
            0),
        ::stbi_image_free);

    if (raw_data == nullptr)
    {
        throw iris::Exception("failed to load image");
    }

    // calculate the total number of bytes needed for the raw data
    const auto size = width * height * num_channels;

    // take a copy of the image data
    auto image_data = std::vector<std::uint8_t>(
        static_cast<std::uint8_t *>(raw_data.get()),
        static_cast<std::uint8_t *>(raw_data.get()) + size);

    return std::make_tuple(
        std::move(image_data),
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height),
        static_cast<std::uint32_t>(num_channels));
}

}

namespace iris::texture_factory
{

Texture *load(const std::string &resource)
{
    if (cache.count(resource) == 0)
    {
        const auto file_data = ResourceLoader::instance().load(resource);
        const auto [data, width, height, num_channels] = parse_image(file_data);

        auto format = PixelFormat::RGB;
        switch (num_channels)
        {
            case 3:
                format = PixelFormat::RGB;
                break;
            case 4:
                format = PixelFormat::RGBA;
                break;
            default:
                throw Exception("unsupported number of channels");
        }

        cache[resource] =
            std::make_unique<Texture>(data, width, height, format);
    }

    return cache[resource].get();
}

Texture *create(
    const std::vector<std::uint8_t> &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
{
    std::stringstream strm;
    strm << "!" << counter;
    ++counter;

    const auto resource = strm.str();

    cache[resource] =
        std::make_unique<Texture>(data, width, height, pixel_format);

    return cache[resource].get();
}

Texture *blank()
{
    static auto texture = Texture::blank();
    return &texture;
}

}
