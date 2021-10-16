////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/renderer.h"

#include "core/exception.h"

namespace iris
{

Renderer::Renderer()
    : render_passes_()
    , render_queue_()
    , post_processing_scene_()
    , post_processing_target_(nullptr)
    , post_processing_camera_()
{
}

void Renderer::render()
{
    pre_render();

    // call each command with the appropriate handler
    for (auto &command : render_queue_)
    {
        switch (command.type())
        {
            case RenderCommandType::UPLOAD_TEXTURE: execute_upload_texture(command); break;
            case RenderCommandType::PASS_START: execute_pass_start(command); break;
            case RenderCommandType::DRAW: execute_draw(command); break;
            case RenderCommandType::PASS_END: execute_pass_end(command); break;
            case RenderCommandType::PRESENT: execute_present(command); break;
            default: throw Exception("unknown render queue command");
        }
    }

    post_render();
}

void Renderer::pre_render()
{
    // default is to do nothing
}

void Renderer::execute_upload_texture(RenderCommand &)
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
