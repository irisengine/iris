////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "metal_gui_renderer.h"

#include <CoreText/CoreText.h>
#include <cstdint>
#include <format>
#include <functional>
#include <memory>

#include <Metal/Metal.h>
#include <optional>
#include <string>

#include "imgui.h"

#include "ImGuizmo.h"
#include "backends/imgui_impl_metal.h"
#include "core/context.h"
#include "core/macos/macos_ios_utility.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "events/keyboard_event.h"
#include "events/mouse_button_event.h"
#include "graphics/metal/metal_renderer.h"
#include "graphics/render_command.h"
#include "graphics/render_entity_type.h"
#include "graphics/renderer.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "graphics/texture_manager.h"
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

std::string label_name(std::string_view label, std::uint32_t id)
{
    return std::string{label} + std::to_string(id);
}

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
    iris::Context &ctx,
    std::uint32_t width,
    std::uint32_t height,
    iris::Scene *scene,
    iris::Camera &camera)
    : iris::Renderer(ctx.material_manager())
    , impl_(std::make_unique<implementation>())
    , width_(width)
    , height_(height)
    , show_demo_(false)
    , camera_(camera)
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
        ctx.texture_manager(),
        ctx.material_manager(),
        width,
        height,
        [&, scene]
        {
            static std::vector<iris::SingleEntity *> entities;

            impl_->io().DisplaySize = ImVec2(width_, height_);
            impl_->io().DeltaTime = 1.0f / 30.0f;

            ::ImGui_ImplMetal_NewFrame(impl_->renderer->single_pass_descriptor());
            ::ImGui::NewFrame();

            ::ImGuizmo::SetOrthographic(false);
            ::ImGuizmo::BeginFrame();

            if (show_demo_)
            {
                ::ImGui::ShowDemoWindow(nullptr);
            }

            ::ImGui::Begin("Object creator", nullptr, ImGuiWindowFlags_None);
            if (::ImGui::Button("Add Box"))
            {
                entities.push_back(scene->create_entity<iris::SingleEntity>(
                    nullptr, ctx.mesh_manager().cube(iris::Colour{1.0f, 1.0f, 1.0f}), iris::Transform{{}, {}, {1.0f}}));
            }

            auto counter = 0u;
            for (auto *entity : entities)
            {
                if (::ImGui::TreeNode(label_name("Object", counter).c_str()))
                {
                    auto position = entity->position();
                    float pos_x = position.x;
                    float pos_y = position.y;
                    float pos_z = position.z;

                    ::ImGui::LabelText("", "Position");
                    {
                        ::ImGui::Text("X:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##px", counter).c_str(), &pos_x);
                    }
                    {
                        ::ImGui::Text("Y:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##py", counter).c_str(), &pos_y);
                    }
                    {
                        ::ImGui::Text("Z:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##pz", counter).c_str(), &pos_z);
                    }

                    const iris::Vector3 new_position{pos_x, pos_y, pos_z};
                    if (new_position != position)
                    {
                        entity->set_position(new_position);
                    }

                    auto rotation = entity->orientation();
                    auto [rot_x, rot_y, rot_z] = rotation.to_euler_angles();

                    ::ImGui::LabelText("", "Rotation");
                    {
                        ::ImGui::Text("X:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##rx", counter).c_str(), &rot_x, 0.5f);
                    }
                    {
                        ::ImGui::Text("Y:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##ry", counter).c_str(), &rot_y, 0.5f);
                    }
                    {
                        ::ImGui::Text("Z:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##rz", counter).c_str(), &rot_z, 0.5f);
                    }

                    const iris::Quaternion new_rotation{rot_x, rot_y, rot_z};
                    if (new_rotation != rotation)
                    {
                        entity->set_orientation(new_rotation);
                    }

                    auto scale = entity->scale();
                    float scale_x = scale.x;
                    float scale_y = scale.y;
                    float scale_z = scale.z;

                    ::ImGui::LabelText("", "Scale");
                    {
                        ::ImGui::Text("X:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##sx", counter).c_str(), &scale_x);
                    }
                    {
                        ::ImGui::Text("Y:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##sy", counter).c_str(), &scale_y);
                    }
                    {
                        ::ImGui::Text("Z:");
                        ::ImGui::SameLine();
                        ::ImGui::DragFloat(label_name("##sz", counter).c_str(), &scale_z);
                    }

                    const iris::Vector3 new_scale{scale_x, scale_y, scale_z};
                    if (new_scale != scale)
                    {
                        entity->set_scale(new_scale);
                    }
                    ::ImGui::TreePop();
                }

                ++counter;
            }

            ::ImGuizmo::Enable(true);
            ::ImGuizmo::SetRect(0, 0, impl_->io().DisplaySize.x, impl_->io().DisplaySize.y);

            static const float identityMatrix[16] = {
                1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};
            auto inv_view = iris::Matrix4::transpose(camera_.view());
            const auto inv_proj = iris::Matrix4::transpose(camera_.projection());
            ::ImGuizmo::DrawGrid(inv_view.data(), inv_proj.data(), identityMatrix, 100.f);

            if (!entities.empty())
            {
                auto transform = iris::Matrix4::transpose(entities[0]->transform());
                auto *transform_ptr = transform.data();
                ::ImGuizmo::Manipulate(
                    inv_view.data(),
                    inv_proj.data(),
                    ::ImGuizmo::TRANSLATE,
                    ::ImGuizmo::WORLD,
                    const_cast<float *>(transform_ptr),
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr);

                entities[0]->set_transform(iris::Matrix4::transpose(transform));
            }

            ::ImGui::End();

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
    else if (event.is_key(iris::Key::I, iris::KeyState::DOWN))
    {
        show_demo_ = !show_demo_;
    }
}

void MetalGuiRenderer::do_set_render_pipeline(std::function<void()> build_queue)
{
    impl_->renderer->do_set_render_pipeline(build_queue);
}
