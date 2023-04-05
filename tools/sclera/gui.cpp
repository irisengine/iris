////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "gui.h"

#include <deque>
#include <optional>
#include <ranges>
#include <vector>

#include "imgui.h"

#include "ImGuizmo.h"
#include "core/auto_release.h"
#include "core/context.h"
#include "entity.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "graphics/window.h"
#include "graphics/yaml_scene_loader.h"
#include "log/log.h"
#include "yaml-cpp/yaml.h"

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

struct AutoSequence
{
    AutoSequence(::YAML::Emitter &out)
        : out_(out)
    {
        out_ << ::YAML::BeginSeq;
    }

    ~AutoSequence()
    {
        out_ << ::YAML::EndSeq;
    }

    ::YAML::Emitter &out_;
};

struct AutoMap
{
    AutoMap(::YAML::Emitter &out)
        : out_(out)
    {
        out_ << ::YAML::BeginMap;
    }

    ~AutoMap()
    {
        out_ << ::YAML::EndMap;
    }

    ::YAML::Emitter &out_;
};

YAML::Emitter &operator<<(YAML::Emitter &out, const iris::Vector3 &v)
{
    out << YAML::Flow;
    AutoSequence seq{out};
    out << v.x << v.y << v.z;

    return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const iris::Quaternion &q)
{
    out << YAML::Flow;
    AutoSequence seq{out};
    out << q.x << q.y << q.z << q.w;

    return out;
}

template <class T>
void serialise_yaml_key_value(::YAML::Emitter &out, const std::string &key, const T &value)
{
    out << ::YAML::Key;
    out << key;
    out << ::YAML::Value;
    out << value;
}

std::string label_name(std::string_view label, std::uint32_t id)
{
    return std::string{label} + std::to_string(id);
}

void object_creator_ui(
    iris::Context &ctx,
    std::deque<Entity> &entities,
    std::unordered_map<Entity *, std::function<void()>> &entity_creators,
    iris::Scene *scene,
    Entity **selected_entity)
{
    if (::ImGui::Button("Add Box"))
    {
        const auto creator = [&]
        {
            auto *entity = scene->create_entity<iris::SingleEntity>(
                nullptr, ctx.mesh_manager().cube(iris::Colour{1.0f, 1.0f, 1.0f}), iris::Transform{{}, {}, {1.0f}});

            return Entity{std::vector{entity}, ""};
        };

        auto &new_entity = entities.emplace_back(creator());
        *selected_entity = std::addressof(new_entity);
        entity_creators[*selected_entity] = creator;
    }

    ::ImGui::SameLine();

    if (::ImGui::Button("Add model"))
    {
        ImGui::OpenPopup("model_select_popup");
    }

    if (ImGui::BeginPopup("model_select_popup"))
    {
        const auto is_fbx = [](const auto &str) { return str.ends_with(".fbx"); };

        for (auto model : ctx.resource_manager().available_resources() | std::ranges::views::filter(is_fbx))
        {
            if (ImGui::Selectable(model.c_str()))
            {
                const auto creator = [model, scene, selected_entity, &ctx, &entities]
                {
                    const auto mesh_parts = ctx.mesh_manager().load_mesh(model);
                    std::vector<iris::SingleEntity *> engine_entities{};

                    for (const auto &mesh : mesh_parts.mesh_data)
                    {
                        engine_entities.push_back(scene->create_entity<iris::SingleEntity>(
                            nullptr, mesh.mesh, iris::Transform{{}, {}, {1.0f}}));
                    }

                    auto &new_entity = entities.emplace_back(engine_entities, model);
                    *selected_entity = std::addressof(new_entity);
                    (*selected_entity)
                        ->set_transform(
                            iris::Transform{{}, {{1.0f, 0.0f, 0.0f}, -std::numbers::pi_v<float> / 2.0f}, {1.0f}});
                };

                creator();
                entity_creators[*selected_entity] = creator;
            }
        }
        ImGui::EndPopup();
    }
}

void object_editor_tree_ui(std::deque<Entity> &entities)
{
    auto counter = 0u;
    for (auto &entity : entities)
    {
        auto [position, rotation, scale] = entity.transform().decompose();
        auto dirty = false;

        if (::ImGui::TreeNode(label_name("Object", counter).c_str()))
        {
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
                position = new_position;
                dirty = true;
            }

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
                rotation = new_rotation;
                dirty = true;
            }

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
                scale = new_scale;
                dirty = true;
            }
            ::ImGui::TreePop();
        }

        if (dirty)
        {
            entity.set_transform({position, rotation, scale});
        }

        ++counter;
    }
}

void selected_object_gizmo_ui(
    ::ImGuiIO &io,
    Entity *selected,
    const iris::Camera &camera,
    ::ImGuizmo::OPERATION transform_operation)
{
    ::ImGuizmo::SetOrthographic(false);
    ::ImGuizmo::BeginFrame();

    ::ImGuizmo::Enable(true);
    ::ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    static const std::array<float, 16> identity_matrix = {
        {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f}};
    auto inv_view = iris::Matrix4::transpose(camera.view());
    const auto inv_proj = iris::Matrix4::transpose(camera.projection());
    ::ImGuizmo::DrawGrid(inv_view.data(), inv_proj.data(), identity_matrix.data(), 100.f);

    if (selected != nullptr)
    {
        auto transform = iris::Matrix4::transpose(selected->transform().matrix());
        auto *transform_ptr = transform.data();

        const auto snap_value = transform_operation == ::ImGuizmo::ROTATE ? 45.0f : 1.0f;
        std::array<float, 3u> snap = {snap_value, snap_value, snap_value};

        ::ImGuizmo::Manipulate(
            inv_view.data(),
            inv_proj.data(),
            transform_operation,
            ::ImGuizmo::WORLD,
            const_cast<float *>(transform_ptr),
            nullptr,
            snap.data(),
            nullptr,
            nullptr);

        selected->set_transform(iris::Transform{iris::Matrix4::transpose(transform)});
    }
}

void save_scene(iris::Context &ctx, std::deque<Entity> &entities)
{
    ::YAML::Emitter out;

    AutoMap models{out};
    out << ::YAML::Key << "models";
    out << ::YAML::Value;

    AutoSequence seq{out};
    for (const auto &entity : entities)
    {
        AutoMap map{out};

        const auto [position, rotation, scale] = entity.transform().decompose();

        serialise_yaml_key_value(out, "file_name", entity.file_name());
        serialise_yaml_key_value(out, "position", position);
        serialise_yaml_key_value(out, "rotation", rotation);
        serialise_yaml_key_value(out, "scale", scale);
    }

    const auto *string_ptr = out.c_str();
    iris::DataBuffer data(std::strlen(string_ptr));
    std::memcpy(data.data(), string_ptr, std::strlen(string_ptr));

    ctx.resource_manager().save("scene.yml", data);
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
    if (ctx.resource_manager().exists("scene.yml"))
    {
        iris::YamlSceneLoader loader{iris_ctx_, "scene.yml"};
        loader.load(
            scene, [this](const auto &entities, auto file_name) { entities_.emplace_back(entities, file_name); });
    }

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

        object_creator_ui(iris_ctx_, entities_, entity_creators_, scene_, &selected_);
        object_editor_tree_ui(entities_);
        selected_object_gizmo_ui(io_, selected_, camera_, transform_operation_);
    }

    ::ImGui::Render();
    post_render();
}

void Gui::handle_input(const iris::Event &event)
{
    static auto x = window_->width() / 2.0f;
    static auto y = window_->height() / 2.0f;
    static auto control = false;

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

        if (event.is_mouse_button(iris::MouseButton::LEFT, iris::MouseButtonState::DOWN) && !is_mouse_captured())
        {
            const iris::Vector3 mouse_coord{
                (2.0f * x) / window_->width() - 1.0f, 1.0f - (2.0f * y) / window_->height(), 1.0f};

            const auto to_world = iris::Matrix4::invert(camera_.projection() * camera_.view());

            auto from = to_world * iris::Vector4(mouse_coord.x, mouse_coord.y, -1.0f, 1.0f);
            auto to = to_world * iris::Vector4(mouse_coord.x, mouse_coord.y, 1.0f, 1.0f);
            from /= from.w;
            to /= to.w;

            const auto origin = from.xyz();
            const auto direction = iris::Vector3::normalise(to.xyz() - from.xyz());
            const auto radius = std::numbers::sqrt2_v<float>;

            selected_ = nullptr;

            for (auto &entity : entities_)
            {
                const auto centre = entity.transform().translation();

                const auto b = direction.dot(origin - centre);
                const auto c = (origin - centre).dot(origin - centre) - std::pow(radius, 2.0f);

                const auto t = (std::pow(b, 2.0f) - c);

                if (t >= 0.0f)
                {
                    selected_ = std::addressof(entity);
                    break;
                }
            }
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
                case D:
                {
                    if ((selected_ != nullptr) && control)
                    {
                        const auto creator = entity_creators_[selected_];
                        creator();
                        entity_creators_[selected_] = creator;
                    }
                    break;
                }
                case S:
                {
                    if (control)
                    {
                        save_scene(iris_ctx_, entities_);
                    }
                    break;
                }
                case FORWARD_DELETE:
                {
                    if (selected_ != nullptr)
                    {
                        for (auto *entity : selected_->entities())
                        {
                            scene_->remove(entity);
                        }

                        std::erase_if(entities_, [this](const Entity &e) { return std::addressof(e) == selected_; });
                        selected_ = nullptr;
                    }

                    break;
                }
                case CONTROL: [[fallthrough]];
                case COMMAND: control = true; break;
                default: break;
            }
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

bool Gui::is_mouse_captured() const
{
    return io_.WantCaptureMouse;
}
