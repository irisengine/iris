#pragma once

#include <cstdint>
#include <memory>

#include "core/data_buffer.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"

namespace iris
{

/**
 * Implementation of TextureManager for d3d12.
 */
class D3D12TextureManager : public TextureManager
{
  public:
    ~D3D12TextureManager() override = default;

  protected:
    /**
     * Create a Texture object with the provided data.
     *
     * @param data
     *   Raw data of image, in pixel_format.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param usage
     *   Usage of the texture.
     */
    std::unique_ptr<Texture> do_create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        TextureUsage usage) override;
};

}
