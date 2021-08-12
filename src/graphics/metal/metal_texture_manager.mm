#include "graphics/metal/metal_texture_manager.h"

#include <cstdint>
#include <memory>

#include "core/data_buffer.h"
#include "graphics/metal/metal_texture.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace iris
{
std::unique_ptr<Texture> MetalTextureManager::create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
{
    return std::make_unique<MetalTexture>(data, width, height, pixel_format);
}

}
