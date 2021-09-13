#include "graphics/d3d12/d3d12_render_target.h"

#include <cstdint>
#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>

#include "core/root.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/texture.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace
{

iris::D3D12DescriptorHandle create_render_target(
    const iris::D3D12Texture *texture,
    bool is_multisampled)
{
    auto *device = iris::D3D12Context::device();

    const auto handle = iris::D3D12DescriptorManager::cpu_allocator(
                            D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
                            .allocate_static();

    D3D12_RENDER_TARGET_VIEW_DESC descriptor = {0};
    descriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (is_multisampled)
    {
        descriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
    }
    else
    {
        descriptor.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        descriptor.Texture2D.MipSlice = 0u;
        descriptor.Texture2D.PlaneSlice = 0u;
    }

    device->CreateRenderTargetView(
        texture->resource(), &descriptor, handle.cpu_handle());

    return handle;
}

}

namespace iris
{

D3D12RenderTarget::D3D12RenderTarget(
    std::unique_ptr<D3D12Texture> colour_texture,
    std::unique_ptr<D3D12Texture> depth_texture,
    std::uint32_t samples)
    : RenderTarget(std::move(colour_texture), std::move(depth_texture))
    , handle_()
    , multisample_handle_()
    , multisample_colour_texture_()
    , multisample_depth_texture_()
{
    colour_texture_->set_flip(true);
    depth_texture_->set_flip(true);

    const auto scale = Root::window_manager().current_window()->screen_scale();

    const auto byte = static_cast<std::byte>(0xff);
    const DataBuffer texture_data(
        colour_texture_->width() * scale * colour_texture_->height() * scale *
            4u,
        byte);

    multisample_colour_texture_ = std::make_unique<D3D12Texture>(
        texture_data,
        colour_texture_->width(),
        colour_texture_->height(),
        colour_texture_->pixel_format(),
        true,
        samples);
    multisample_colour_texture_->set_flip(true);

    multisample_depth_texture_ = std::make_unique<D3D12Texture>(
        depth_texture_->width(), depth_texture_->height(), samples);
    multisample_colour_texture_->set_flip(true);

    handle_ = create_render_target(
        static_cast<D3D12Texture *>(colour_texture_.get()), false);
    multisample_handle_ =
        create_render_target(multisample_colour_texture_.get(), true);
}

D3D12DescriptorHandle D3D12RenderTarget::handle() const
{
    return handle_;
}

D3D12DescriptorHandle D3D12RenderTarget::multisample_handle() const
{
    return multisample_handle_;
}

D3D12Texture *D3D12RenderTarget::multisample_colour_texture() const
{
    return multisample_colour_texture_.get();
}

D3D12Texture *D3D12RenderTarget::multisample_depth_texture() const
{
    return multisample_depth_texture_.get();
}

}
