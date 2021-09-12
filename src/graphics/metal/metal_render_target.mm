#include "graphics/metal/metal_render_target.h"

#include <cstdint>
#include <memory>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "graphics/metal/metal_texture.h"
#include "graphics/texture.h"

namespace iris
{

MetalRenderTarget::MetalRenderTarget(
    std::unique_ptr<MetalTexture> colour_texture,
    std::unique_ptr<MetalTexture> depth_texture,
    std::uint32_t samples)
    : RenderTarget(std::move(colour_texture), std::move(depth_texture))
    , multisample_colour_texture_()
    , multisample_depth_texture_()
{
    colour_texture_->set_flip(true);
    depth_texture_->set_flip(true);

    if (samples > 1u)
    {
        multisample_colour_texture_ = std::make_unique<MetalTexture>(
            DataBuffer{},
            colour_texture_->width(),
            colour_texture_->height(),
            colour_texture_->pixel_format(),
            samples);
        multisample_colour_texture_->set_flip(true);

        multisample_depth_texture_ = std::make_unique<MetalTexture>(
            DataBuffer{},
            depth_texture_->width(),
            depth_texture_->height(),
            depth_texture_->pixel_format(),
            samples);
        multisample_depth_texture_->set_flip(true);
    }
}

MetalTexture *MetalRenderTarget::multisample_colour_texture() const
{
    return multisample_colour_texture_.get();
}

MetalTexture *MetalRenderTarget::multisample_depth_texture() const
{
    return multisample_depth_texture_.get();
}

}
