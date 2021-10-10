#include "graphics/texture_manager.h"

#include <cstdint>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/auto_release.h"
#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "core/resource_loader.h"
#include "graphics/texture.h"
#include "graphics/texture_usage.h"

namespace
{

/**
 * Load an image from a data buffer.
 *
 * @param data
 *   Image data.
 *
 * @returns
 *   Tuple of <data, width, height, number of channels>.
 */
std::tuple<iris::DataBuffer, std::uint32_t, std::uint32_t> parse_image(
    const iris::DataBuffer &data)
{
    int width = 0;
    int height = 0;
    int num_channels = 0;

    // ensure that images are flipped along the y axis when loaded, this is so
    // they work with what the graphics api treats as the origin
    ::stbi_set_flip_vertically_on_load(true);

    // load image using stb library
    iris::AutoRelease<::stbi_uc *, nullptr> raw_data(
        ::stbi_load_from_memory(
            reinterpret_cast<const stbi_uc *>(data.data()),
            static_cast<int>(data.size()),
            &width,
            &height,
            &num_channels,
            0),
        ::stbi_image_free);

    iris::ensure(raw_data && (num_channels != 0), "failed to load image");

    // calculate the total number of bytes needed for the raw data
    const auto size = width * height * num_channels;

    static constexpr auto output_channels = 4u;

    // create buffer big enough for RGBA data
    iris::DataBuffer padded_data{width * height * output_channels};

    // we only store image data as RGBA in the engine, so extend the data if
    // we have less than four channels

    auto dst_ptr = padded_data.data();
    auto *src_ptr = reinterpret_cast<const std::byte *>(raw_data.get());
    const auto *end_ptr =
        reinterpret_cast<const std::byte *>(raw_data.get() + size);

    while (src_ptr != end_ptr)
    {
        // default pixel value (black with alpha)
        // this allows us to memcpy over the data we do have and leaves the
        // correct defaults if we have less than four channels
        std::byte rgba[] = {
            std::byte{0x0}, std::byte{0x0}, std::byte{0x0}, std::byte{0xff}};

        std::memcpy(rgba, src_ptr, num_channels);
        std::memcpy(dst_ptr, rgba, output_channels);

        dst_ptr += output_channels;
        src_ptr += num_channels;
    }

    return std::make_tuple(
        std::move(padded_data),
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
}

}

namespace iris
{

Texture *TextureManager::load(const std::string &resource, TextureUsage usage)
{
    expect(
        (usage == TextureUsage::IMAGE) || (usage == TextureUsage::DATA),
        "can only load IMAGE or DATA from file");

    // check if texture has been loaded before, if not then load it
    auto loaded = loaded_textures_.find(resource);
    if (loaded == std::cend(loaded_textures_))
    {
        const auto file_data = ResourceLoader::instance().load(resource);
        auto [data, width, height] = parse_image(file_data);

        auto texture = do_create(data, width, height, usage);

        loaded_textures_[resource] = {1u, std::move(texture)};
    }
    else
    {
        ++loaded_textures_[resource].ref_count;
    }

    return loaded_textures_[resource].texture.get();
}

Texture *TextureManager::create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    TextureUsage usage)
{
    static std::uint32_t counter = 0u;

    // create a unique name for the in-memory texture
    std::stringstream strm;
    strm << "!" << counter;
    ++counter;

    const auto resource = strm.str();

    auto texture = do_create(data, width, height, usage);

    loaded_textures_[resource] = {1u, std::move(texture)};

    return loaded_textures_[resource].texture.get();
}

void TextureManager::unload(Texture *texture)
{
    // allow for implementation specific unloading logic
    destroy(texture);

    // don't unload the static blank texture!
    if (texture != blank())
    {
        // find the texture that we want to unload
        auto loaded = std::find_if(
            std::begin(loaded_textures_),
            std::end(loaded_textures_),
            [texture](const auto &element)
            { return element.second.texture.get() == texture; });

        expect(
            loaded != std::cend(loaded_textures_),
            "texture has not been loaded");

        // decrement reference count and, if 0, unload
        --loaded->second.ref_count;
        if (loaded->second.ref_count == 0u)
        {
            loaded_textures_.erase(loaded);
        }
    }
}

Texture *TextureManager::blank()
{
    static Texture *texture = create(
        {std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}},
        1u,
        1u,
        TextureUsage::IMAGE);

    return texture;
}

void TextureManager::destroy(Texture *)
{
}

}
