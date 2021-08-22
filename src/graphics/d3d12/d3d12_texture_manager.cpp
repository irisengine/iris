#include "graphics/d3d12/d3d12_texture_manager.h"

#include <cstdint>
#include <memory>

#include "core/data_buffer.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace iris
{
std::unique_ptr<Texture> D3D12TextureManager::create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
{
    return std::make_unique<D3D12Texture>(data, width, height, pixel_format);
}

}
