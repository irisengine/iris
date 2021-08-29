#include "graphics/texture_manager.h"

#include <cstdint>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "core/auto_release.h"
#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/resource_loader.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

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
std::tuple<iris::DataBuffer, std::uint32_t, std::uint32_t, std::uint32_t>
parse_image(const iris::DataBuffer &data)
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

    if (!raw_data)
    {
        throw iris::Exception("failed to load image");
    }

    // calculate the total number of bytes needed for the raw data
    const auto size = width * height * num_channels;

    const auto *raw_data_ptr =
        reinterpret_cast<const std::byte *>(raw_data.get());

    // take a copy of the image data
    auto image_data = iris::DataBuffer(raw_data_ptr, raw_data_ptr + size);

    return std::make_tuple(
        std::move(image_data),
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height),
        static_cast<std::uint32_t>(num_channels));
}

}

namespace iris
{

Texture *TextureManager::load(const std::string &resource)
{
    // check if texture has been loaded before, if not then load it
    auto loaded = loaded_textures_.find(resource);
    if (loaded == std::cend(loaded_textures_))
    {
        const auto file_data = ResourceLoader::instance().load(resource);
        auto [data, width, height, num_channels] = parse_image(file_data);

        // some graphics apis (e.g. metal) do not support RGB natively, so we
        // always extend the data to have a fourth component (alpha - always 1)
        if (num_channels == 3u)
        {
            // create buffer big enough for RGBA data filled with 255 so we
            // don't have to worry about setting the alpha
            DataBuffer padded_data{
                width * height * 4u, static_cast<std::byte>(255u)};

            auto dst_ptr = padded_data.data();
            auto src_ptr = data.data();

            while (src_ptr != data.data() + data.size())
            {
                // copy RGB then skip past RGBA
                std::memcpy(dst_ptr, src_ptr, 3u);
                dst_ptr += 4u;
                src_ptr += 3u;
            }

            data = padded_data;
        }

        auto texture = create(data, width, height, PixelFormat::RGBA);

        loaded_textures_[resource] = {1u, std::move(texture)};
    }
    else
    {
        ++loaded_textures_[resource].ref_count;
    }

    return loaded_textures_[resource].texture.get();
}

Texture *TextureManager::load(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
{
    static std::uint32_t counter = 0u;

    // create a unique name for the in-memory texture
    std::stringstream strm;
    strm << "!" << counter;
    ++counter;

    const auto resource = strm.str();

    auto texture = create(data, width, height, pixel_format);

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
            [texture](const auto &element) {
                return element.second.texture.get() == texture;
            });

        if (loaded == std::cend(loaded_textures_))
        {
            throw Exception("texture has not been loaded");
        }

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
    static const auto byte = static_cast<std::byte>(0xFF);
    static Texture *texture =
        load({byte, byte, byte, byte}, 1u, 1u, PixelFormat::RGBA);
    return texture;
}

void TextureManager::destroy(Texture *)
{
}

}
