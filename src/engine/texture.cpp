#include "texture.hpp"

#include <cstdint>
#include <experimental/filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "gl/texture_implementation.hpp"

namespace eng
{

texture::texture(const std::experimental::filesystem::path &path)
    : data_(),
      width_(0u),
      height_(0u),
      num_channels_(0u),
      impl_(nullptr)
{
    if(!std::experimental::filesystem::exists(path))
    {
        throw std::runtime_error(path.string() + " does not exist");
    }

    int width = 0;
    int height = 0;
    int num_channels = 0;

    // load image using stb library
    std::unique_ptr<::stbi_uc, decltype(&::stbi_image_free)> raw_data(
        ::stbi_load(path.c_str(), &width, &height, &num_channels, 0),
        ::stbi_image_free);

    if(raw_data == nullptr)
    {
        throw std::runtime_error("failed to load image");
    }

    // cast data members
    width_ = static_cast<std::uint32_t>(width);
    height_ = static_cast<std::uint32_t>(height);
    num_channels_ = static_cast<std::uint32_t>(num_channels);

    // calculate the total number of bytes needed for the raw data
    const auto size = width_ * height_ * num_channels_;

    // take a copy of the image data
    data_ = std::vector<std::uint8_t>(
        static_cast<std::uint8_t*>(raw_data.get()),
        static_cast<std::uint8_t*>(raw_data.get()) + size);

    impl_ = std::make_unique<gl::texture_implementation>(
        data_,
        width_,
        height_,
        num_channels_);
}

texture::texture(
    const std::vector<std::uint8_t> &data,
    const std::uint32_t width,
    const std::uint32_t height,
    const std::uint32_t num_channels)
    : data_(data),
      width_(width),
      height_(height),
      num_channels_(num_channels),
      impl_()
{
    const auto expected_size = width * height * num_channels;
    if(data.size() != expected_size)
    {
        throw std::runtime_error("incorrect data size");
    }

    impl_ = std::make_unique<gl::texture_implementation>(
        data_,
        width_,
        height_,
        num_channels_);
}

void texture::bind() const
{
    impl_->bind();
}

void texture::unbind() const
{
    impl_->unbind();
}

std::vector<std::uint8_t> texture::data() const noexcept
{
    return data_;
}

std::uint32_t texture::width() const noexcept
{
    return width_;
}

std::uint32_t texture::height() const noexcept
{
    return height_;
}

std::uint32_t texture::num_channels() const noexcept
{
    return num_channels_;
}

}

