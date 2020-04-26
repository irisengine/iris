#include "platform/window.h"

#include <memory>

#import <UIKit/UIKit.h>

#include "graphics/font.h"

namespace eng
{

Window::Window(
    const float width,
    const float height)
    : render_system_(),
      width_(width),
      height_(height)
{
    const auto bounds = [[UIScreen mainScreen] bounds];
    render_system_ = std::make_unique<RenderSystem>(bounds.size.width, bounds.size.height);
}

void Window::render()
{
    render_system_->render();
}

Sprite* Window::create(
    const float x,
    const float y,
    const float width,
    const float height,
    const Vector3 &colour)
{
    return render_system_->create(x, y, width, height, colour);
}

Sprite* Window::create(
    const float x,
    const float y,
    const float width,
    const float height,
    const Vector3 &colour,
    Texture &&tex)
{
    return render_system_->create(x, y, width, height, colour, std::move(tex));
}

Sprite* Window::create(
    const std::string &font_name,
    const std::uint32_t size,
    const Vector3 &colour,
    const std::string &text,
    const float x,
    const float y)
{
    const Font fnt{ font_name, size, colour };
    return render_system_->add(fnt.sprite(text, x, y));
}

std::optional<Event> Window::pump_event()
{
    return { };
}

}
