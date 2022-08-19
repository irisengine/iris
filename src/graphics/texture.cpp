////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/texture.h"

#include <cstdint>

#include "graphics/sampler.h"
#include "graphics/texture_usage.h"

namespace iris
{

Texture::Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    const Sampler *sampler,
    TextureUsage usage,
    std::uint32_t index)
    : data_(data)
    , width_(width)
    , height_(height)
    , sampler_(sampler)
    , usage_(usage)
    , index_(index)
    , has_transparency_(false)
{
    for (auto i = 3u; i < data_.size(); i += 4u)
    {
        if (data_[i] != std::byte{0xFF})
        {
            has_transparency_ = true;
            break;
        }
    }
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

const Sampler *Texture::sampler() const
{
    return sampler_;
}

TextureUsage Texture::usage() const
{
    return usage_;
}

std::uint32_t Texture::index() const
{
    return index_;
}

bool Texture::has_transparency() const
{
    return has_transparency_;
}

}
