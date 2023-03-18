////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "metal_gui_renderer.h"

#include <CoreText/CoreText.h>
#include <cstdint>
#include <functional>
#include <memory>

#include <Metal/Metal.h>
#include <optional>

#include "backends/imgui_impl_metal.h"
#include "core/macos/macos_ios_utility.h"
#include "events/mouse_button_event.h"
#include "graphics/metal/metal_renderer.h"
#include "graphics/render_command.h"
#include "graphics/renderer.h"
#include "graphics/texture_manager.h"
#include "imgui.h"
#include "log/log.h"

namespace
{
class ActualMetalGuiRenderer : public iris::MetalRenderer
{
  public:
    ActualMetalGuiRenderer(
        iris::TextureManager &texture_manager,
        iris::MaterialManager &material_manager,
        std::uint32_t width,
        std::uint32_t height,
        std::function<void()> execute_pass_end_hook)
        : iris::MetalRenderer(texture_manager, material_manager, width, height)
        , execute_pass_end_hook_(execute_pass_end_hook)
    {
    }

    ~ActualMetalGuiRenderer()
    {
    }

    void pre_render() override
    {
        iris::MetalRenderer::pre_render();
    }

    void execute_pass_start(iris::RenderCommand &command) override
    {
        iris::MetalRenderer::execute_pass_start(command);
    }

    void execute_draw(iris::RenderCommand &command) override
    {
        iris::MetalRenderer::execute_draw(command);
    }

    void execute_pass_end(iris::RenderCommand &command) override
    {
        execute_pass_end_hook_();
        iris::MetalRenderer::execute_pass_end(command);
    }

    void execute_present(iris::RenderCommand &command) override
    {
        iris::MetalRenderer::execute_present(command);
    }

    void post_render() override
    {
        iris::MetalRenderer::post_render();
    }

    void do_set_render_pipeline(std::function<void()> build_queue) override
    {
        iris::MetalRenderer::do_set_render_pipeline(build_queue);
    }

    MTLRenderPassDescriptor *single_pass_descriptor() const
    {
        return single_pass_descriptor_;
    }

    id<MTLCommandBuffer> command_buffer() const
    {
        return command_buffer_;
    }

    id<MTLRenderCommandEncoder> render_encoder() const
    {
        return render_encoder_;
    }

  private:
    std::function<void()> execute_pass_end_hook_;
};

}

struct MetalGuiRenderer::implementation
{
    implementation()
    {
        IMGUI_CHECKVERSION();

        ::ImGui::CreateContext();
        imgui_io = std::make_unique<std::reference_wrapper<ImGuiIO>>(::ImGui::GetIO());
    }

    ImGuiIO &io()
    {
        return imgui_io->get();
    }

    std::unique_ptr<std::reference_wrapper<ImGuiIO>> imgui_io;
    id<MTLCommandQueue> command_queue;
    MTLRenderPassDescriptor *pass_descriptor;
    std::unique_ptr<ActualMetalGuiRenderer> renderer;
};

MetalGuiRenderer::MetalGuiRenderer(
    iris::TextureManager &texture_manager,
    iris::MaterialManager &material_manager,
    std::uint32_t width,
    std::uint32_t height)
    : iris::Renderer(material_manager)
    , impl_(std::make_unique<implementation>())
    , width_(width)
    , height_(height)
{
    impl_->io().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    impl_->io().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    impl_->io().ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    impl_->io().MouseDrawCursor = true;
    impl_->io().DisplaySize = ImVec2(width_ * 2.0f, height * 2.0f);
    impl_->io().DisplayFramebufferScale = ImVec2(2.0f, 2.0f);

    ::ImGui::StyleColorsDark();

    unsigned char *tex_pixels = nullptr;
    int tex_w, tex_h;
    impl_->io().Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

    auto device = iris::core::utility::metal_device();
    ::ImGui_ImplMetal_Init(device);

    impl_->command_queue = [device newCommandQueue];
    impl_->pass_descriptor = [MTLRenderPassDescriptor new];
    impl_->renderer = std::make_unique<ActualMetalGuiRenderer>(
        texture_manager,
        material_manager,
        width,
        height,
        [&]
        {
            impl_->io().DisplaySize = ImVec2(width_, height_);
            impl_->io().DeltaTime = 1.0f / 30.0f;

            ::ImGui_ImplMetal_NewFrame(impl_->renderer->single_pass_descriptor());
            ::ImGui::NewFrame();

            ::ImGui::ShowDemoWindow(nullptr);

            ::ImGui::Render();
            ::ImGui_ImplMetal_RenderDrawData(
                ::ImGui::GetDrawData(), impl_->renderer->command_buffer(), impl_->renderer->render_encoder());
        });
}

MetalGuiRenderer::~MetalGuiRenderer()
{
    ::ImGui_ImplMetal_Shutdown();
    ::ImGui::DestroyContext();
}

void MetalGuiRenderer::pre_render()
{
    impl_->renderer->pre_render();
}

void MetalGuiRenderer::execute_pass_start(iris::RenderCommand &command)
{
    impl_->renderer->execute_pass_start(command);
}

void MetalGuiRenderer::execute_draw(iris::RenderCommand &command)
{
    impl_->renderer->execute_draw(command);
}

void MetalGuiRenderer::execute_pass_end(iris::RenderCommand &command)
{
    impl_->renderer->execute_pass_end(command);
}

void MetalGuiRenderer::execute_present(iris::RenderCommand &command)
{
    impl_->renderer->execute_present(command);
}

void MetalGuiRenderer::post_render()
{
    impl_->renderer->post_render();
}

void MetalGuiRenderer::handle_input(iris::Event event)
{
    static auto x = width_ / 2.0f;
    static auto y = height_ / 2.0f;

    if (event.is_mouse())
    {
        const auto mouse_event = event.mouse();
        x += mouse_event.delta_x;
        y += mouse_event.delta_y;

        impl_->io().AddMousePosEvent(x, y);
    }
    else if (event.is_mouse_button())
    {
        const auto mouse_button = event.mouse_button();
        std::optional<int> imgui_button;
        std::optional<bool> imgui_state;

        switch (mouse_button.button)
        {
            case iris::MouseButton::LEFT: imgui_button = 0; break;
            case iris::MouseButton::RIGHT: imgui_button = 1; break;
        }

        switch (mouse_button.state)
        {
            case iris::MouseButtonState::UP: imgui_state = false; break;
            case iris::MouseButtonState::DOWN: imgui_state = true; break;
        }

        if (imgui_button && imgui_state)
        {
            impl_->io().AddMouseButtonEvent(*imgui_button, *imgui_state);
        }
    }
}

void MetalGuiRenderer::do_set_render_pipeline(std::function<void()> build_queue)
{
    impl_->renderer->do_set_render_pipeline(build_queue);
}
