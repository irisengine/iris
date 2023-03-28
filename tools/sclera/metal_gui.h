////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gui.h"

#include <functional>

#include <Metal/Metal.h>

#include "core/camera.h"
#include "core/context.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "graphics/window.h"

class MetalGui : public Gui
{
  public:
    MetalGui(
        iris::Context &ctx,
        const iris::Window *window,
        iris::Scene *scene,
        iris::Camera &camera,
        MTLRenderPassDescriptor *pass_descriptor,
        std::function<id<MTLCommandBuffer>()> get_command_buffer,
        std::function<id<MTLRenderCommandEncoder>()> get_render_encoder);
    ~MetalGui() override;

  protected:
    void pre_render() override;
    void post_render() override;

  private:
    id<MTLCommandQueue> command_queue_;
    MTLRenderPassDescriptor *pass_descriptor_;
    std::function<id<MTLCommandBuffer>()> get_command_buffer_;
    std::function<id<MTLRenderCommandEncoder>()> get_render_encoder_;
};
