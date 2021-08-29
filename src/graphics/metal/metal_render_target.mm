#include "graphics/metal/metal_render_target.h"

#include <memory>

#import <Metal/Metal.h>

#include "graphics/texture.h"

namespace iris
{

MetalRenderTarget::MetalRenderTarget(
    std::unique_ptr<MetalTexture> colour_texture,
    std::unique_ptr<MetalTexture> depth_texture)
    : RenderTarget(std::move(colour_texture), std::move(depth_texture))
{
    colour_texture_->set_flip(true);
    depth_texture_->set_flip(true);
}

}
