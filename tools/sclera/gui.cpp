////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "gui.h"

#include <optional>
#include <vector>

#include "imgui.h"

#include "ImGuizmo.h"
#include "core/auto_release.h"
#include "core/context.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "graphics/window.h"

namespace
{

auto create_imgui_context()
{
    IMGUI_CHECKVERSION();

    return iris::AutoRelease<::ImGuiContext *, nullptr>{::ImGui::CreateContext(), ::ImGui::DestroyContext};
}

std::optional<ImGuiKey> iris_to_imgui_key(iris::Key key)
{
    switch (key)
    {
        using enum iris::Key;

        case TAB: return ImGuiKey_Tab;
        case LEFT_ARROW: return ImGuiKey_LeftArrow;
        case RIGHT_ARROW: return ImGuiKey_RightArrow;
        case UP_ARROW: return ImGuiKey_UpArrow;
        case DOWN_ARROW: return ImGuiKey_DownArrow;
        case PAGE_UP: return ImGuiKey_PageUp;
        case PAGE_DOWN: return ImGuiKey_PageDown;
        case HOME: return ImGuiKey_Home;
        case END: return ImGuiKey_End;
        case SPACE: return ImGuiKey_Space;
        case ESCAPE: return ImGuiKey_Escape;
        case CONTROL: return ImGuiKey_LeftCtrl;
        case SHIFT: return ImGuiKey_LeftShift;
        case NUM_0: return ImGuiKey_0;
        case NUM_1: return ImGuiKey_1;
        case NUM_2: return ImGuiKey_2;
        case NUM_3: return ImGuiKey_3;
        case NUM_4: return ImGuiKey_4;
        case NUM_5: return ImGuiKey_5;
        case NUM_6: return ImGuiKey_6;
        case NUM_7: return ImGuiKey_7;
        case NUM_8: return ImGuiKey_8;
        case NUM_9: return ImGuiKey_9;
        case A: return ImGuiKey_A;
        case B: return ImGuiKey_B;
        case C: return ImGuiKey_C;
        case D: return ImGuiKey_D;
        case E: return ImGuiKey_E;
        case F: return ImGuiKey_F;
        case G: return ImGuiKey_G;
        case H: return ImGuiKey_H;
        case I: return ImGuiKey_I;
        case J: return ImGuiKey_J;
        case K: return ImGuiKey_K;
        case L: return ImGuiKey_L;
        case M: return ImGuiKey_M;
        case N: return ImGuiKey_N;
        case O: return ImGuiKey_O;
        case P: return ImGuiKey_P;
        case Q: return ImGuiKey_Q;
        case R: return ImGuiKey_R;
        case S: return ImGuiKey_S;
        case T: return ImGuiKey_T;
        case U: return ImGuiKey_U;
        case V: return ImGuiKey_V;
        case W: return ImGuiKey_W;
        case X: return ImGuiKey_X;
        case Y: return ImGuiKey_Y;
        case Z: return ImGuiKey_Z;
        case F1: return ImGuiKey_F1;
        case F2: return ImGuiKey_F2;
        case F3: return ImGuiKey_F3;
        case F4: return ImGuiKey_F4;
        case F5: return ImGuiKey_F5;
        case F6: return ImGuiKey_F6;
        case F7: return ImGuiKey_F7;
        case F8: return ImGuiKey_F8;
        case F9: return ImGuiKey_F9;
        case F10: return ImGuiKey_F10;
        case F11: return ImGuiKey_F11;
        case F12: return ImGuiKey_F12;
        case COMMA: return ImGuiKey_Comma;
        case MINUS: return ImGuiKey_Minus;
        case PERIOD: return ImGuiKey_Period;
        case SLASH: return ImGuiKey_Slash;
        case SEMI_COLON: return ImGuiKey_Semicolon;
        case EQUAL: return ImGuiKey_Equal;
        case LEFT_BRACKET: return ImGuiKey_LeftBracket;
        case BACKSLASH: return ImGuiKey_Backslash;
        case RIGHT_BRACKET: return ImGuiKey_RightBracket;
        case CAPS_LOCK: return ImGuiKey_CapsLock;
        case KEYPAD_0: return ImGuiKey_Keypad0;
        case KEYPAD_1: return ImGuiKey_Keypad1;
        case KEYPAD_2: return ImGuiKey_Keypad2;
        case KEYPAD_3: return ImGuiKey_Keypad3;
        case KEYPAD_4: return ImGuiKey_Keypad4;
        case KEYPAD_5: return ImGuiKey_Keypad5;
        case KEYPAD_6: return ImGuiKey_Keypad6;
        case KEYPAD_7: return ImGuiKey_Keypad7;
        case KEYPAD_8: return ImGuiKey_Keypad8;
        case KEYPAD_9: return ImGuiKey_Keypad9;
        case KEYPAD_DECIMAL: return ImGuiKey_KeypadDecimal;
        case KEYPAD_DIVIDE: return ImGuiKey_KeypadDivide;
        case KEYPAD_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case KEYPAD_MINUS: return ImGuiKey_KeypadSubtract;
        case KEYPAD_PLUS: return ImGuiKey_KeypadAdd;
        case KEYPAD_ENTER: return ImGuiKey_KeypadEnter;
        default: return std::nullopt;
    }
}

struct AutoBegin
{
    template <class... Args>
    AutoBegin(Args &&...args)
    {
        ::ImGui::Begin(std::forward<Args>(args)...);
    }

    ~AutoBegin()
    {
        ::ImGui::End();
    }
};

std::string label_name(std::string_view label, std::uint32_t id)
{
    return std::string{label} + std::to_string(id);
}

void object_creator_ui(iris::Context &ctx, std::vector<iris::SingleEntity *> &entities, iris::Scene *scene)
{
    if (::ImGui::Button("Add Box"))
    {
        entities.push_back(scene->create_entity<iris::SingleEntity>(
            nullptr, ctx.mesh_manager().cube(iris::Colour{1.0f, 1.0f, 1.0f}), iris::Transform{{}, {}, {1.0f}}));
    }
}

void object_editor_tree_ui(std::vector<iris::SingleEntity *> &entities)
{
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
}

void selected_object_gizmo_ui(
    ::ImGuiIO &io,
    const std::vector<iris::SingleEntity *> &entities,
    const iris::Camera &camera,
    ::ImGuizmo::OPERATION transform_operation)
{
    ::ImGuizmo::SetOrthographic(false);
    ::ImGuizmo::BeginFrame();

    ::ImGuizmo::Enable(true);
    ::ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    static const float identityMatrix[16] = {
        1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    auto inv_view = iris::Matrix4::transpose(camera.view());
    const auto inv_proj = iris::Matrix4::transpose(camera.projection());
    ::ImGuizmo::DrawGrid(inv_view.data(), inv_proj.data(), identityMatrix, 100.f);

    const auto m = iris::Matrix4::transpose(iris::Transform({}, {}, {1.0f}).matrix());
    ::ImGuizmo::DrawCubes(inv_view.data(), inv_proj.data(), m.data(), 1);

    if (!entities.empty())
    {
        auto transform = iris::Matrix4::transpose(entities[0]->transform());
        auto *transform_ptr = transform.data();
        ::ImGuizmo::Manipulate(
            inv_view.data(),
            inv_proj.data(),
            transform_operation,
            ::ImGuizmo::WORLD,
            const_cast<float *>(transform_ptr),
            nullptr,
            nullptr,
            nullptr,
            nullptr);

        entities[0]->set_transform(iris::Matrix4::transpose(transform));
    }
}

}

Gui::Gui(iris::Context &ctx, const iris::Window *window, iris::Scene *scene, iris::Camera &camera)
    : iris_ctx_(ctx)
    , imgui_ctx_(create_imgui_context())
    , io_(::ImGui::GetIO())
    , window_(window)
    , scene_(scene)
    , camera_(camera)
    , entities_()
    , show_demo_(false)
    , transform_operation_(::ImGuizmo::TRANSLATE)
{
    const auto scale = window_->screen_scale();

    io_.ConfigFlags |= ::ImGuiConfigFlags_NavEnableKeyboard;
    io_.ConfigFlags |= ::ImGuiConfigFlags_NavEnableGamepad;
    io_.ConfigFlags |= ::ImGuiConfigFlags_NavEnableSetMousePos;
    io_.MouseDrawCursor = true;
    io_.DisplaySize = ::ImVec2(static_cast<float>(window_->width()), static_cast<float>(window_->height()));
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

    if (show_demo_)
    {
        ::ImGui::ShowDemoWindow(nullptr);
    }

    {
        AutoBegin begin{"Object creator", nullptr, ImGuiWindowFlags_None};

        object_creator_ui(iris_ctx_, entities_, scene_);
        object_editor_tree_ui(entities_);
        selected_object_gizmo_ui(io_, entities_, camera_, transform_operation_);
    }

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
    else if (event.is_key())
    {
        const auto key = event.key();
        if (key.state == iris::KeyState::DOWN)
        {
            switch (key.key)
            {
                using enum iris::Key;
                case TAB: show_demo_ = !show_demo_; break;
                case W: transform_operation_ = ::ImGuizmo::TRANSLATE; break;
                case E: transform_operation_ = ::ImGuizmo::ROTATE; break;
                case R: transform_operation_ = ::ImGuizmo::SCALE; break;
                default: break;
            }
        }
        if ((key.key == iris::Key::TAB) && (key.state == iris::KeyState::DOWN))
        {
            show_demo_ = !show_demo_;
        }

        if (const auto imgui_key = iris_to_imgui_key(key.key); imgui_key)
        {
            io_.AddKeyEvent(*imgui_key, key.state == iris::KeyState::DOWN);
        }
    }
    else if (event.is_scroll_wheel())
    {
        const auto scroll = event.scroll_wheel();
        io_.AddMouseWheelEvent(0.0f, scroll.delta_y);
    }
}
