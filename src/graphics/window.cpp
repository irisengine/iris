////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/window.h"

#include <cstdint>
#include <deque>
#include <memory>

#include "graphics/render_pass.h"
#include "graphics/render_pipeline.h"
#include "graphics/render_target.h"

namespace iris
{

Window::Window(std::uint32_t width, std::uint32_t height)
    : width_(width)
    , height_(height)
    , renderer_(nullptr)
{
}

void Window::render() const
{
    renderer_->render();
}

std::uint32_t Window::width() const
{
    return width_;
}

std::uint32_t Window::height() const
{
    return height_;
}

void Window::set_render_pipeline(std::unique_ptr<RenderPipeline> render_pipeline)
{
    render_pipeline->clear_dirty_bit();
    renderer_->set_render_pipeline(std::move(render_pipeline));
}

}
