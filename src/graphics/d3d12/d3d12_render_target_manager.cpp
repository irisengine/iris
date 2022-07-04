////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_render_target_manager.h"

#include "core/root.h"
#include "graphics/d3d12/d3d12_render_target.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/sampler.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

RenderTarget *D3D12RenderTargetManager::create()
{
    const auto *window = Root::window_manager().current_window();
    return create(window->width(), window->height());
}

RenderTarget *D3D12RenderTargetManager::create(std::uint32_t width, std::uint32_t height)
{
    const auto scale = Root::window_manager().current_window()->screen_scale();
    const auto *sampler = Root::texture_manager().create(SamplerDescriptor{.uses_mips = false});
    const auto *depth_sampler = Root::texture_manager().create(SamplerDescriptor{
        .s_address_mode = SamplerAddressMode::CLAMP_TO_BORDER,
        .t_address_mode = SamplerAddressMode::CLAMP_TO_BORDER,
        .border_colour = Colour{1.0f, 1.0f, 1.0f, 1.0f},
        .uses_mips = false});

    return render_targets_
        .emplace_back(std::make_unique<D3D12RenderTarget>(
            Root::texture_manager().create(
                DataBuffer{}, width * scale, height * scale, TextureUsage::RENDER_TARGET, sampler),
            Root::texture_manager().create(
                DataBuffer{}, width * scale, height * scale, TextureUsage::DEPTH, depth_sampler)))
        .get();
}

RenderTarget *D3D12RenderTargetManager::create(const RenderTarget *colour_target, const RenderTarget *depth_target)
{
    return render_targets_
        .emplace_back(
            std::make_unique<D3D12RenderTarget>(colour_target->colour_texture(), depth_target->depth_texture()))
        .get();
}

}
