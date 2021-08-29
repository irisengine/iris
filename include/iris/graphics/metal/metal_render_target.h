#pragma once

#include <memory>

#include "graphics/metal/metal_texture.h"
#include "graphics/render_target.h"

namespace iris
{

/**
 * Implementation of RenderTarget for metal.
 */
class MetalRenderTarget : public RenderTarget
{
  public:
    /**
     * Construct a new MetalRenderTarget.
     *
     * @param colour_texture
     *   Texture to render colour data to.
     *
     * @param depth_texture
     *   Texture to render depth data to.
     */
    MetalRenderTarget(
        std::unique_ptr<MetalTexture> colour_texture,
        std::unique_ptr<MetalTexture> depth_texture);

    ~MetalRenderTarget() override = default;
};

}
