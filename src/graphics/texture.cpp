#include "graphics/texture.h"

#include <memory>

#include "graphics/texture_usage.h"

namespace iris
{

Texture::Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    TextureUsage usage)
    : data_(data)
    , width_(width)
    , height_(height)
    , flip_(false)
    , usage_(usage)
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

TextureUsage Texture::usage() const
{
    return usage_;
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
