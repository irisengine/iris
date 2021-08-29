#include "graphics/texture.h"

#include <memory>

#include "graphics/pixel_format.h"

namespace iris
{

Texture::Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
    : data_(data)
    , width_(width)
    , height_(height)
    , flip_(false)
    , format_(pixel_format)
{
}

Texture::~Texture() = default;

const DataBuffer &Texture::data() const
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

PixelFormat Texture::pixel_format() const
{
    return format_;
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
