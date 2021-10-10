#include "graphics/render_target.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"

namespace iris
{

RenderTarget::RenderTarget(
    std::unique_ptr<Texture> colour_texture,
    std::unique_ptr<Texture> depth_texture)
    : colour_texture_(std::move(colour_texture))
    , depth_texture_(std::move(depth_texture))
{
    expect(
        (colour_texture_->width() == depth_texture_->width()) &&
            (colour_texture_->height() == depth_texture_->height()),
        "colour and depth dimensions must match");
}

RenderTarget::~RenderTarget() = default;

Texture *RenderTarget::colour_texture() const
{
    return colour_texture_.get();
}

Texture *RenderTarget::depth_texture() const
{
    return depth_texture_.get();
}

std::uint32_t RenderTarget::width() const
{
    return colour_texture_->width();
}

std::uint32_t RenderTarget::height() const
{
    return colour_texture_->height();
}

}
