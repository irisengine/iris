////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_texture_manager.h"

#include <cstdint>
#include <memory>

#include "core/data_buffer.h"
#include "graphics/metal/metal_texture.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"

namespace iris
{
std::unique_ptr<Texture> MetalTextureManager::do_create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    TextureUsage usage)
{
    return std::make_unique<MetalTexture>(data, width, height, usage);
}

}
