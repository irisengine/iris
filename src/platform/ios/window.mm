#include "platform/window.h"

#include <memory>

#import <UIKit/UIKit.h>

#import "platform/ios/MetalViewController.h"
#include "graphics/font.h"
#include "log/log.h"

namespace iris
{

struct Window::implementation
{
};

Window::Window(float width, float height)
    : width_(width),
      height_(height),
      render_system_(nullptr),
      impl_(nullptr)
{
    const auto bounds = [[UIScreen mainScreen] bounds];
    width_ = bounds.size.width;
    height_ = bounds.size.height;

    // we can now create a render system
    render_system_ = std::make_unique<RenderSystem>(width_, height_);
}

Window::~Window() = default;

std::optional<Event> Window::pump_event()
{
    const CFTimeInterval seconds = 0.000002;

    // run the default loop, this pumps touch events which will then be picked
    // up by our view
    auto result = kCFRunLoopRunHandledSource;
    do
    {
        result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, TRUE);
    } while(result == kCFRunLoopRunHandledSource);
    
    const auto *window = [[[UIApplication sharedApplication] windows] objectAtIndex:0];
    const auto *root_view_controller = static_cast<MetalViewController*>([window rootViewController]);
    
    std::optional<Event> event;
    
    // get next event from view (if one is available)
    if(!root_view_controller->events_.empty())
    {
        event = root_view_controller->events_.front();
        root_view_controller->events_.pop();
    }
    
    return event;
}

void Window::render(const Pipeline &pipeline) const
{
    render_system_->render(pipeline);
}

float Window::width() const
{
    return width_;
}

float Window::height() const
{
    return height_;
}

}
