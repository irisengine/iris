////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <deque>
#include <functional>
#include <unordered_map>

#include "imgui.h"

#include "ImGuizmo.h"
#include "core/auto_release.h"
#include "core/camera.h"
#include "core/context.h"
#include "entity.h"
#include "events/event.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "graphics/window.h"

class Gui
{
  public:
    Gui(iris::Context &ctx, const iris::Window *window, iris::Scene *scene, iris::Camera &camera);
    virtual ~Gui() = default;
    void render();

    void handle_input(const iris::Event &event);

    bool is_mouse_captured() const;

  protected:
    virtual void pre_render() = 0;
    virtual void post_render() = 0;

    iris::Context &iris_ctx_;
    iris::AutoRelease<::ImGuiContext *, nullptr> imgui_ctx_;
    ::ImGuiIO &io_;
    const iris::Window *window_;
    iris::Scene *scene_;
    iris::Camera &camera_;
    std::deque<Entity> entities_;
    bool show_demo_;
    ::ImGuizmo::OPERATION transform_operation_;
    Entity *selected_;
    std::unordered_map<Entity *, std::function<void()>> entity_creators_;
};
