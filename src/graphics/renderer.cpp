////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/renderer.h"

#include <cassert>

#include "core/exception.h"
#include "core/root.h"
#include "graphics/material_manager.h"

namespace iris
{

Renderer::Renderer()
    : render_queue_()
    , render_pipeline_()
    , start_(std::chrono::steady_clock::now())
    , time_(0u)
{
}

void Renderer::render()
{
    if (render_pipeline_->is_dirty())
    {
        render_queue_ = render_pipeline_->rebuild();
        render_pipeline_->clear_dirty_bit();
    }

    pre_render();

    // update time
    // this is calculated here rather than in time() so all calls to time() produce the same value for a given frame
    time_ = std::chrono::steady_clock::now() - start_;

    // call each command with the appropriate handler
    for (auto &command : render_queue_)
    {
        switch (command.type())
        {
            case RenderCommandType::PASS_START: execute_pass_start(command); break;
            case RenderCommandType::DRAW: execute_draw(command); break;
            case RenderCommandType::PASS_END: execute_pass_end(command); break;
            case RenderCommandType::PRESENT: execute_present(command); break;
            default: throw Exception("unknown render queue command");
        }
    }

    post_render();
}

void Renderer::set_render_pipeline(std::unique_ptr<RenderPipeline> render_pipeline)
{
    Root::material_manager().clear();
    start_ = std::chrono::steady_clock::now();

    render_pipeline_ = std::move(render_pipeline);
    do_set_render_pipeline([this]() {
        render_queue_ = render_pipeline_->build();
        render_pipeline_->clear_dirty_bit();
    });
}

std::chrono::milliseconds Renderer::time() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time_);
}

void Renderer::pre_render()
{
    // default is to do nothing
}

void Renderer::execute_pass_start(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_draw(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_pass_end(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_present(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::post_render()
{
    // default is to do nothing
}

}
