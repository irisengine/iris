////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/texture.h"

#include <cstdint>

#include "graphics/texture_usage.h"

namespace iris
{

Texture::Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    TextureUsage usage,
    std::uint32_t index)
    : data_(data)
    , width_(width)
    , height_(height)
    , flip_(false)
    , usage_(usage)
    , index_(index)
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

std::uint32_t Texture::index() const
{
    return index_;
}

}
