////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

#include "graphics/render_command.h"
#include "graphics/render_command_type.h"
#include "graphics/render_pass.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"

class FakeRenderer : public iris::Renderer
{
  public:
    FakeRenderer(const std::vector<iris::RenderCommand> &render_queue)
        : iris::Renderer()
    {
        render_queue_ = render_queue;
    }

    std::vector<iris::RenderCommandType> call_log() const
    {
        return call_log_;
    }

    ~FakeRenderer() override = default;

    // overridden methods which just log when they are called

    void pre_render() override
    {
    }

    void execute_pass_start(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::PASS_START);
    }

    void execute_draw(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::DRAW);
    }

    void execute_pass_end(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::PASS_END);
    }

    void execute_present(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::PRESENT);
    }

    void post_render() override
    {
    }

  protected:
    void do_set_render_pipeline(std::function<void()> build_queue)
    {
        build_queue();
    }

  private:
    std::vector<iris::RenderCommandType> call_log_;
};