////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_render_target.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>

#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/texture.h"

namespace iris
{

D3D12RenderTarget::D3D12RenderTarget(const D3D12Texture *colour_texture, const D3D12Texture *depth_texture)
    : RenderTarget(colour_texture, depth_texture)
    , handle_()
{
    colour_texture->set_flip(true);
    depth_texture->set_flip(true);

    auto *device = D3D12Context::device();

    handle_ = D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).allocate_static();

    device->CreateRenderTargetView(
        static_cast<const D3D12Texture *>(colour_texture_)->resource(), nullptr, handle_.cpu_handle());
}

D3D12DescriptorHandle D3D12RenderTarget::handle() const
{
    return handle_;
}

}
