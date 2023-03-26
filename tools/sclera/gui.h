////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/auto_release.h"
#include "events/event.h"
#include "graphics/window.h"
#include "imgui.h"

class Gui
{
  public:
    Gui(const iris::Window *window);
    virtual ~Gui() = default;
    void render();

    void handle_input(const iris::Event &event);

  protected:
    virtual void pre_render() = 0;
    virtual void post_render() = 0;

    iris::AutoRelease<::ImGuiContext *, nullptr> ctx_;
    ::ImGuiIO &io_;
    const iris::Window *window_;
};
