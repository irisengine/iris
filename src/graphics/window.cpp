#include "graphics/window.h"

#include <cstdint>
#include <vector>

#include "graphics/render_pass.h"
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

RenderTarget *Window::create_render_target()
{
    return create_render_target(width_, height_);
}

RenderTarget *Window::create_render_target(
    std::uint32_t width,
    std::uint32_t height)
{
    return renderer_->create_render_target(width, height);
}

void Window::set_render_passes(const std::vector<RenderPass> &render_passes)
{
    renderer_->set_render_passes(render_passes);
}

}
