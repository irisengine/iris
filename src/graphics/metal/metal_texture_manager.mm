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
