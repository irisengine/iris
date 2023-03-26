////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "metal_gui.h"

#include <functional>

#include <Metal/Metal.h>

#include "imgui.h"

#include "backends/imgui_impl_metal.h"
#include "core/macos/macos_ios_utility.h"
#include "graphics/window.h"

MetalGui::MetalGui(
    const iris::Window *window,
    MTLRenderPassDescriptor *pass_descriptor,
    std::function<id<MTLCommandBuffer>()> get_command_buffer,
    std::function<id<MTLRenderCommandEncoder>()> get_render_encoder)
    : Gui(window)
    , command_queue_(nullptr)
    , pass_descriptor_(pass_descriptor)
    , get_command_buffer_(get_command_buffer)
    , get_render_encoder_(get_render_encoder)
{
    auto device = iris::core::utility::metal_device();
    ::ImGui_ImplMetal_Init(device);

    command_queue_ = [device newCommandQueue];
}

MetalGui::~MetalGui()
{
    ::ImGui_ImplMetal_Shutdown();
}

void MetalGui::pre_render()
{
    ::ImGui_ImplMetal_NewFrame(pass_descriptor_);
}

void MetalGui::post_render()
{
    ::ImGui_ImplMetal_RenderDrawData(::ImGui::GetDrawData(), get_command_buffer_(), get_render_encoder_());
}