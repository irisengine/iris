////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "gui.h"

#include "core/auto_release.h"
#include "graphics/window.h"
#include "imgui.h"

namespace
{

auto create_imgui_context()
{
    IMGUI_CHECKVERSION();

    return iris::AutoRelease<::ImGuiContext *, nullptr>{::ImGui::CreateContext(), ::ImGui::DestroyContext};
}

}

Gui::Gui(const iris::Window *window)
    : ctx_(create_imgui_context())
    , io_(::ImGui::GetIO())
    , window_(window)
{
    const auto scale = window_->screen_scale();

    io_.ConfigFlags |= ::ImGuiConfigFlags_NavEnableKeyboard;
    io_.ConfigFlags |= ::ImGuiConfigFlags_NavEnableGamepad;
    io_.ConfigFlags |= ::ImGuiConfigFlags_NavEnableSetMousePos;
    io_.MouseDrawCursor = true;
    io_.DisplaySize =
        ::ImVec2(static_cast<float>(window_->width() * scale), static_cast<float>(window_->height() * scale));
    io_.DisplayFramebufferScale = ::ImVec2(static_cast<float>(scale), static_cast<float>(scale));
    io_.DeltaTime = 1.0f / 30.0f;

    ::ImGui::StyleColorsDark();

    unsigned char *tex_pixels = nullptr;
    auto tex_w = 0;
    auto tex_h = 0;
    io_.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
}

void Gui::render()
{
    pre_render();
    ::ImGui::NewFrame();

    ::ImGui::ShowDemoWindow(nullptr);

    //::ImGui::End();
    ::ImGui::Render();
    post_render();
}

void Gui::handle_input(const iris::Event &event)
{
    static auto x = window_->width() / 2.0f;
    static auto y = window_->height() / 2.0f;

    if (event.is_mouse())
    {
        const auto mouse_event = event.mouse();
        x += mouse_event.delta_x;
        y += mouse_event.delta_y;

        io_.AddMousePosEvent(x, y);
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
            io_.AddMouseButtonEvent(*imgui_button, *imgui_state);
        }
    }
    else if (event.is_key(iris::Key::I, iris::KeyState::DOWN))
    {
        // show_demo_ = !show_demo_;
    }
}
