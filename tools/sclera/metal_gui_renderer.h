////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>

#include "core/context.h"
#include "events/event.h"
#include "graphics/render_command.h"
#include "graphics/renderer.h"
#include "graphics/scene.h"

class MetalGuiRenderer : public iris::Renderer
{
  public:
    MetalGuiRenderer(iris::Context &ctx, std::uint32_t width, std::uint32_t height, iris::Scene *scene);
    ~MetalGuiRenderer();
    MetalGuiRenderer(const MetalGuiRenderer &) = delete;
    MetalGuiRenderer &operator=(const MetalGuiRenderer &) = delete;

    // handlers for the supported RenderCommandTypes

    void pre_render() override;
    void execute_pass_start(iris::RenderCommand &command) override;
    void execute_draw(iris::RenderCommand &command) override;
    void execute_pass_end(iris::RenderCommand &command) override;
    void execute_present(iris::RenderCommand &command) override;
    void post_render() override;

    void handle_input(iris::Event event);

  private:
    void do_set_render_pipeline(std::function<void()> build_queue) override;

    struct implementation;
    std::unique_ptr<implementation> impl_;

    std::uint32_t width_;
    std::uint32_t height_;
    bool show_demo_;
};