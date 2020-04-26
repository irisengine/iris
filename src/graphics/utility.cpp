#include "graphics/utility.h"

#include <cstdint>
#include <filesystem>
#include <tuple>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "core/exception.h"

namespace eng::graphics::utility
{

std::tuple<
    std::vector<std::uint8_t>,
    std::uint32_t,
    std::uint32_t,
    std::uint32_t>
    load_image(const std::filesystem::path &path)
{
    if(!std::filesystem::exists(path))
    {
        throw Exception(path.string() + " does not exist");
    }

    int width = 0;
    int height = 0;
    int num_channels = 0;

    // ensure that images are flipped along the y axis when loaded, this is so
    // they work with what the graphics api treats as the origin
    ::stbi_set_flip_vertically_on_load(true);

    // load image using stb library
    std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)> raw_data(
        ::stbi_load(path.c_str(), &width, &height, &num_channels, 0),
        ::stbi_image_free);

    if(raw_data == nullptr)
    {
        throw Exception("failed to load image");
    }

    // calculate the total number of bytes needed for the raw data
    const auto size = width * height * num_channels;

    // take a copy of the image data
    auto data = std::vector<std::uint8_t>(
        static_cast<std::uint8_t*>(raw_data.get()),
        static_cast<std::uint8_t*>(raw_data.get()) + size);

    return std::make_tuple(
        std::move(data),
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height),
        static_cast<std::uint32_t>(num_channels));
}

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

    if(raw_data == nullptr)
    {
        throw Exception("failed to load image");
    }

    // calculate the total number of bytes needed for the raw data
    const auto size = width * height * num_channels;

    // take a copy of the image data
    auto image_data = std::vector<std::uint8_t>(
        static_cast<std::uint8_t*>(raw_data.get()),
        static_cast<std::uint8_t*>(raw_data.get()) + size);

    return std::make_tuple(
        std::move(image_data),
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height),
        static_cast<std::uint32_t>(num_channels));
}

}


