////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_texture_manager.h"

#include <cstdint>
#include <memory>

#include "core/data_buffer.h"
#include "graphics/cube_map.h"
#include "graphics/d3d12/d3d12_cube_map.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"

namespace iris
{

std::unique_ptr<Texture> D3D12TextureManager::do_create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    TextureUsage usage)

{
    return std::make_unique<D3D12Texture>(data, width, height, usage);
}

std::unique_ptr<CubeMap> D3D12TextureManager::do_create(
    const DataBuffer &right_data,
    const DataBuffer &left_data,
    const DataBuffer &top_data,
    const DataBuffer &bottom_data,
    const DataBuffer &near_data,
    const DataBuffer &far_data,
    std::uint32_t width,
    std::uint32_t height)
{
    return std::make_unique<D3D12CubeMap>(
        right_data, left_data, top_data, bottom_data, near_data, far_data, width, height);
}

}
